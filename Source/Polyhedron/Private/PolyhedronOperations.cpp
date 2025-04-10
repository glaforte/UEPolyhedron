// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "PolyhedronOperations.h"
#include "PolyhedronTools.h"

// This structure is intended to remain similar to the Polyhedronisme "poly_flag" structure.
// It helps with porting the operation code. Some operations have been optimized to avoid this structure.
struct FPolyhedronOperationFlagHelper {
  void AddWorkVertex(int64 VertexId, const FVector& Position);
  void AddWorkFlag(int64 FaceId, int64 VertexId1, int64 VertexId2);
  FPolyhedronMesh ConvertWorkBuffers();

  TMap<int64, FVector> WorkVertexPositions;
  TMap<int64, int64> WorkVertexIndices;
  TMap<int64, TMap<int64, int64>> WorkPolygonFlags;
};

void FPolyhedronOperationFlagHelper::AddWorkVertex(int64 VertexId, const FVector& Position) {
  if (!WorkVertexIndices.Contains(VertexId)) {
    WorkVertexIndices.Add(VertexId, static_cast<int64>(0)); // Indexed at a later time?
    WorkVertexPositions.Add(VertexId, Position);
  }
  WorkVertexPositions[VertexId] = Position;
}

void FPolyhedronOperationFlagHelper::AddWorkFlag(int64 FaceId, int64 VertexId1, int64 VertexId2) {
  if (!WorkPolygonFlags.Contains(FaceId)) {
    WorkPolygonFlags.Add(FaceId, TMap<int64, int64>());
  }
  WorkPolygonFlags[FaceId].FindOrAdd(VertexId1) = VertexId2;
}

FPolyhedronMesh FPolyhedronOperationFlagHelper::ConvertWorkBuffers() {

  // A Flag is an associative triple of a face index and two adjacent vertex vertidxs,
  // listed in geometric clockwise order (staring into the normal)
  //
  // Face_i -> V_i -> V_j
  //
  // They are a useful abstraction for defining topological transformations of the polyhedral mesh, as
  // one can refer to vertices and faces that don't yet exist or haven't been traversed yet in the
  // transformation code.
  //
  // A flag is similar in concept to a directed edge.

  FPolyhedronMesh Output;
  Output.Vertices.Reserve(WorkVertexIndices.Num());

  // Number the vertices.
  for (TPair<int64, int64>& Iterator : WorkVertexIndices) {
    Iterator.Get<1>() = Output.Vertices.Num();
    Output.Vertices.Add(WorkVertexPositions[Iterator.Get<0>()]);
  }

  // Build the faces from the poly-flags.
  for (TPair<int64, TMap<int64, int64>>& Iterator : WorkPolygonFlags) {
    TArray<int32> OutputPolygon;
    TMap<int64, int64>& Face = Iterator.Get<1>();

    // Grab any vertex as a starting position.
    int64 Vertex0 = 0;
    for (const TPair<int64, int64>& FaceIterator : Face) {
      Vertex0 = FaceIterator.Get<0>();
      break;
    }

    // Loop through the flags and record the vertex indices.
    int64 VertexIterator = Vertex0;
    OutputPolygon.Add(WorkVertexIndices[VertexIterator]);
    VertexIterator = Face[VertexIterator];
    for (int32 BackstopCounter = 0; BackstopCounter < 1000; ++BackstopCounter) {
      if (VertexIterator == Vertex0) break;
      OutputPolygon.Add(WorkVertexIndices[VertexIterator]);
      VertexIterator = Face[VertexIterator];
    }

    Output.Polygons.Add(OutputPolygon);
  }

  return Output;
}

FPolyhedronMesh FPolyhedronOperations::Dual(const FPolyhedronMesh& Input) {
  // Dual
  // ------------------------------------------------------------------------------------------------
  // The dual of a polyhedron is another mesh wherein:
  // - every face in the original becomes a vertex in the dual
  // - every vertex in the original becomes a face in the dual
  //
  // So N_faces, N_vertices = N_dualfaces, N_dualvertices
  //
  // The new vertex coordinates are convenient to set to the original face centroids.
  //
  FPolyhedronMesh Output;
  int32 InputPolygonCount = Input.Polygons.Num(), OutputVertexCount = InputPolygonCount; // Input Polygon Count -> Output Vertex Count.
  int32 InputVertexCount = Input.Vertices.Num(), OutputPolygonCount = InputVertexCount; // Input Vertex Count -> Output Polygon Count.
  Output.Vertices.SetNum(OutputVertexCount);
  Output.Polygons.SetNum(OutputPolygonCount);

  // Compute the polygon centers: these become the vertices of the new mesh.
  for (int32 InputPolygonIndex = 0; InputPolygonIndex < InputPolygonCount; ++InputPolygonIndex) {
    FVector PolygonCenter = FPolyhedronTools::GetPolygonCenter(Input, Input.Polygons[InputPolygonIndex]);
    Output.Vertices[InputPolygonIndex] = PolygonCenter;
  }

  // Reverse the Extended Mesh' adjacency information to create Output edges.
  FPolyhedronExtendedMesh ExtendedInput = FPolyhedronTools::ComputeEdgeDetails(Input);
  int32 HalfEdgeCount = ExtendedInput.PolygonHalfEdges.Num();

  // Process the output edges into output polygons with preserved winding.
  for (int32 OutputPolygonIndex = 0; OutputPolygonIndex < OutputPolygonCount; ++OutputPolygonIndex) {
    int32 InputVertexIndex = OutputPolygonIndex;
    int32 VertexIteratorStart = ExtendedInput.VertexHalfEdgeOffsets[InputVertexIndex];
    int32 VertexIteratorEnd = ExtendedInput.VertexHalfEdgeOffsets[InputVertexIndex + 1];
    int32 InputVertexPolygonCount = VertexIteratorEnd - VertexIteratorStart;
    int32 OutputPolygonVertexCount = InputVertexPolygonCount;

    Output.Polygons[OutputPolygonIndex].VertexIndices.Reserve(OutputPolygonVertexCount);

    // We can start with any of the half-edges.
    FPolyhedronDirectedHalfEdge& FirstHalfEdge = ExtendedInput.PolygonHalfEdges[ExtendedInput.VertexHalfEdgeIndices[VertexIteratorStart].Get<1>()];
    int32 OutputVertex1 = FirstHalfEdge.PolygonIndexAcross;
    int32 OutputVertex2 = FirstHalfEdge.PolygonIndex;
    Output.Polygons[OutputPolygonIndex].VertexIndices.Add(OutputVertex1);
    Output.Polygons[OutputPolygonIndex].VertexIndices.Add(OutputVertex2);

    // Loop over the other half-edges, looking for the next vertex.
    bool bLoopCompleted = false;
    for (int32 Iterator = VertexIteratorStart + 1; Iterator < VertexIteratorEnd; ++Iterator) {
      int32 NextVertexIndex = -1;
      for (int32 EdgeIndex = 1; EdgeIndex < OutputPolygonVertexCount; ++EdgeIndex) {
        FPolyhedronDirectedHalfEdge& HalfEdge = ExtendedInput.PolygonHalfEdges[ExtendedInput.VertexHalfEdgeIndices[VertexIteratorStart + EdgeIndex].Get<1>()];
        if (HalfEdge.PolygonIndexAcross == OutputVertex2) {
          NextVertexIndex = HalfEdge.PolygonIndex;
          break;
        }
      }
      check(NextVertexIndex != -1);
      if (NextVertexIndex == OutputVertex1) {
        bLoopCompleted = true;
        break;
      }
      Output.Polygons[OutputPolygonIndex].VertexIndices.Add(NextVertexIndex);
      OutputVertex2 = NextVertexIndex;
    }
    // If this check hits, your mesh is not manifold.
    check(bLoopCompleted);
  }

  return Output;
}

FPolyhedronMesh FPolyhedronOperations::Ambo(const FPolyhedronMesh& Input) {
  // Ambo
  // ------------------------------------------------------------------------------------------
  // The best way to think of the ambo operator is as a topological "tween" between a polyhedron
  // and its dual polyhedron.  Thus the ambo of a dual polyhedron is the same as the ambo of the
  // original. Also called "Rectify".
  //
  FPolyhedronOperationFlagHelper PolyFlag;

  int32 VertexCount = Input.Vertices.Num();
  int32 DualPolygonOffset = Input.Polygons.Num();
  auto CalculateMidId = [=] (int32 Vertex1, int32 Vertex2) -> int64 {
    return Vertex1 < Vertex2 ? (static_cast<int64>(Vertex1) * static_cast<int64>(VertexCount) + static_cast<int64>(Vertex2)) : (static_cast<int64>(Vertex2) * static_cast<int64>(VertexCount) + static_cast<int64>(Vertex1));
  };

  // For each face f in the original poly
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    int32 PolygonVertexCount = Polygon.VertexIndices.Num();
    if (PolygonVertexCount < 3) continue;
    int32 Vertex1 = Polygon.VertexIndices[PolygonVertexCount - 2];
    int32 Vertex2 = Polygon.VertexIndices[PolygonVertexCount - 1];
    for (int32 Vertex3 : Polygon.VertexIndices) {
      if (Vertex1 < Vertex2) {
        FVector MidPosition = (Input.Vertices[Vertex1] + Input.Vertices[Vertex2]) / 2.0;
        PolyFlag.AddWorkVertex(CalculateMidId(Vertex1, Vertex2), MidPosition);
      }
      // Add two new flags: one whose face corresponds to the original face
      // and another face that corresponds to (the truncated) v2
      PolyFlag.AddWorkFlag(PolygonIndex, CalculateMidId(Vertex1, Vertex2), CalculateMidId(Vertex2, Vertex3));
      PolyFlag.AddWorkFlag(DualPolygonOffset + Vertex2, CalculateMidId(Vertex2, Vertex3), CalculateMidId(Vertex1, Vertex2));
      // Advance to the next edge pair.
      Vertex1 = Vertex2;
      Vertex2 = Vertex3;
    }
  }

  return PolyFlag.ConvertWorkBuffers();
}

FPolyhedronMesh FPolyhedronOperations::Join(const FPolyhedronMesh& Input) {
  // Leverage the Ambo operation in dual-space.
  FPolyhedronMesh Polyhedron1 = Dual(Input);
  FPolyhedronMesh Polyhedron2 = Ambo(Polyhedron1);
  return Dual(Polyhedron2);
}


FPolyhedronMesh FPolyhedronOperations::Kis(const FPolyhedronMesh& Input, int32 SideFilter, double ApexOffset) {
  // Kis(N)
  // ------------------------------------------------------------------------------------------
  // Kis (abbreviated from triakis) transforms an N-sided face into an N-pyramid rooted at the
  // same base vertices.
  // only kis n-sided faces, but n==0 means kis all.
  //

  // Calculate the apex and added polygon counts.
  int32 InputVertexCount = Input.Vertices.Num();
  int32 ApexStartId = InputVertexCount;
  int32 ApexCount = 0;
  int32 OutputPolygonTotal = 0;
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    if (SideFilter == 0 || SideFilter == Polygon.VertexIndices.Num()) {
      ++ApexCount;
      OutputPolygonTotal += Polygon.VertexIndices.Num();
    } else {
      ++OutputPolygonTotal;
    }
  }

  FPolyhedronMesh Output;
  Output.Vertices.SetNum(InputVertexCount + ApexCount);
  Output.Polygons.SetNum(OutputPolygonTotal);

  // Each old vertex is a new vertex.
  for (int32 InputVertexIndex = 0; InputVertexIndex < Input.Vertices.Num(); ++InputVertexIndex) {
    Output.Vertices[InputVertexIndex] = Input.Vertices[InputVertexIndex];
  }

  int32 NextApexId = ApexStartId;
  int32 NextPolygonId = 0;
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    if (SideFilter == 0 || SideFilter == Polygon.VertexIndices.Num()) {

      Output.Vertices[NextApexId] = FPolyhedronTools::GetPolygonCenter(Input, Polygon) + ApexOffset * FPolyhedronTools::GetPolygonNormal(Input, Polygon);

      int64 Vertex1 = Polygon.VertexIndices.Last(); // Start with the last vertex.
      for (int32 VertexIndex : Polygon.VertexIndices) {
        int64 Vertex2 = VertexIndex;

        Output.Polygons[NextPolygonId].VertexIndices.Add(Vertex1);
        Output.Polygons[NextPolygonId].VertexIndices.Add(Vertex2);
        Output.Polygons[NextPolygonId].VertexIndices.Add(NextApexId);
        ++NextPolygonId;
        Vertex1 = Vertex2;
      }
      ++NextApexId;

    } else {
      Output.Polygons[NextPolygonId].VertexIndices = Polygon.VertexIndices;
      ++NextPolygonId;
    }
  }

  return Output;
}

FPolyhedronMesh FPolyhedronOperations::Needle(const FPolyhedronMesh& Input) {
  // Leverage the Kis operation.
  FPolyhedronMesh Polyhedron1 = Dual(Input);
  return Kis(Polyhedron1, 0, 0.1);
}

FPolyhedronMesh FPolyhedronOperations::Zip(const FPolyhedronMesh& Input) {
  // Leverage the Kis operation.
  FPolyhedronMesh Polyhedron1 = Kis(Input, 0, 0.1);
  return Dual(Polyhedron1);
}

FPolyhedronMesh FPolyhedronOperations::Truncate(const FPolyhedronMesh& Input) {
  // Leverage the Kis operation.
  FPolyhedronMesh Polyhedron1 = Dual(Input);
  FPolyhedronMesh Polyhedron2 = Kis(Polyhedron1, 0, 0.1);
  FPolyhedronMesh Polyhedron3 = Dual(Polyhedron2);

  return Polyhedron3;
}

FPolyhedronMesh FPolyhedronOperations::Chamfer(const FPolyhedronMesh& Input, double Offset) {
  // Chamfer
  // ----------------------------------------------------------------------------------------
  // A truncation along a polyhedron's edges.
  // Chamfering or edge-truncation is similar to expansion, moving faces apart and outward,
  // but also maintains the original vertices. Adds a new hexagonal face in place of each
  // original edge.
  // A polyhedron with e edges will have a chamfered form containing 2e new vertices,
  // 3e new edges, and e new hexagonal faces. -- Wikipedia
  // See also http://dmccooey.com/polyhedra/Chamfer.html
  //

  FPolyhedronOperationFlagHelper PolyFlag;

  // For each face f in the original poly
  int32 InputVertexCount = Input.Vertices.Num();
  int32 InputPolygonCount = Input.Polygons.Num();

  auto CalculateChamferedFaceId = [=] (int64 Vertex1, int64 Vertex2) -> int64 {
    return static_cast<int64>(InputPolygonCount) + (
      (Vertex1 < Vertex2) ?
      (Vertex1 * static_cast<int64>(InputVertexCount) + Vertex2) :
      (Vertex2 * static_cast<int64>(InputVertexCount) + Vertex1)
    );
  };

  for (int32 PolygonIndex = 0; PolygonIndex < InputPolygonCount; ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    if (Polygon.VertexIndices.Num() < 3) continue;

    FVector PolygonNormal = FPolyhedronTools::GetPolygonNormal(Input, Polygon);

    int32 Vertex1 = Polygon.VertexIndices.Last();
    int32 Vertex1New = InputVertexCount + (PolygonIndex * InputVertexCount + Vertex1);

    for (int32 Vertex2 : Polygon.VertexIndices) {
      // Add the original vertex, scaled by the offset slightly (why?)
      PolyFlag.AddWorkVertex(Vertex2, (1.0 + Offset) * Input.Vertices[Vertex2]); // will produce duplicates.
      int32 Vertex2New = InputVertexCount + (PolygonIndex * InputVertexCount + Vertex2);
      PolyFlag.AddWorkVertex(Vertex2New, Input.Vertices[Vertex2] + 1.5 * Offset * PolygonNormal); // magic!
      
      // One whose face corresponds to the original face:
      PolyFlag.AddWorkFlag(PolygonIndex, Vertex1New, Vertex2New);
      // And three for the edges of the new hexagon:
      int64 ChamferedFaceId = CalculateChamferedFaceId(Vertex1, Vertex2);
      PolyFlag.AddWorkFlag(ChamferedFaceId, Vertex2, Vertex2New);
      PolyFlag.AddWorkFlag(ChamferedFaceId, Vertex2New, Vertex1New);
      PolyFlag.AddWorkFlag(ChamferedFaceId, Vertex1New, Vertex1);

      Vertex1 = Vertex2;
      Vertex1New = Vertex2New;
    }
  }

  return PolyFlag.ConvertWorkBuffers();
};


FPolyhedronMesh FPolyhedronOperations::Expand(const FPolyhedronMesh& Input) {
  // Expand is a ambo-ambo combo.
  FPolyhedronMesh Polyhedron1 = Ambo(Input);
  return Ambo(Polyhedron1);
}

FPolyhedronMesh FPolyhedronOperations::Ortho(const FPolyhedronMesh& Input) {
  // Ortho is a join-join combo.
  FPolyhedronMesh Polyhedron1 = Join(Input);
  return Join(Polyhedron1);
}

FPolyhedronMesh FPolyhedronOperations::Gyro(const FPolyhedronMesh& Input) {
  // Gyro
  // ----------------------------------------------------------------------------------------------
  // This is the dual operator to "snub", i.e dual*Gyro = Snub.  It is a bit easier to implement
  // this way.
  //
  // Snub creates at each vertex a new face, expands and twists it, and adds two new triangles to
  // replace each edge.
  //
  FPolyhedronOperationFlagHelper PolyFlag;

  // each old vertex is a new vertex
  int32 InputVertexCount = Input.Vertices.Num();
  for (int32 VertexIndex = 0; VertexIndex < InputVertexCount; ++VertexIndex) {
    PolyFlag.AddWorkVertex(VertexIndex, Input.Vertices[VertexIndex].GetUnsafeNormal());
  }

  // new vertices in center of each face
  int32 CenterOffset = InputVertexCount;
  TArray<FVector> Centers = FPolyhedronTools::GetPolygonCenters(Input);
  int32 InputPolygonCount = Centers.Num();
  for (int32 PolygonIndex = 0; PolygonIndex < InputPolygonCount; ++PolygonIndex) {
    PolyFlag.AddWorkVertex(CenterOffset + PolygonIndex, Centers[PolygonIndex].GetUnsafeNormal());
  }

  int32 GyroVertexOffset = CenterOffset + InputPolygonCount;
  auto CalculateGyroVertexId = [=] (int32 Vertex1, int32 Vertex2) -> int64 {
    return static_cast<int64>(GyroVertexOffset) + static_cast<int64>(Vertex1) * static_cast<int64>(InputVertexCount) + static_cast<int64>(Vertex2);
  };

  for (int32 PolygonIndex = 0; PolygonIndex < InputPolygonCount; ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    int32 PolygonVertexCount = Polygon.VertexIndices.Num();
    if (PolygonVertexCount < 3) continue;
    
    int32 Vertex1 = Polygon.VertexIndices[PolygonVertexCount - 2];
    int32 Vertex2 = Polygon.VertexIndices[PolygonVertexCount - 1];
    for (int32 Vertex3 : Polygon.VertexIndices) {
      int64 FaceId = static_cast<int64>(PolygonIndex) * static_cast<int64>(InputPolygonCount) + static_cast<int64>(Vertex1);
      PolyFlag.AddWorkVertex(CalculateGyroVertexId(Vertex1, Vertex2), FMath::Lerp(Input.Vertices[Vertex1], Input.Vertices[Vertex2], 1.0 / 3.0));
      PolyFlag.AddWorkFlag(FaceId, CenterOffset + PolygonIndex, CalculateGyroVertexId(Vertex1, Vertex2));
      PolyFlag.AddWorkFlag(FaceId, CalculateGyroVertexId(Vertex1, Vertex2), CalculateGyroVertexId(Vertex2, Vertex1));
      PolyFlag.AddWorkFlag(FaceId, CalculateGyroVertexId(Vertex2, Vertex1), Vertex2);
      PolyFlag.AddWorkFlag(FaceId, Vertex2, CalculateGyroVertexId(Vertex2, Vertex3));
      PolyFlag.AddWorkFlag(FaceId, CalculateGyroVertexId(Vertex2, Vertex3), CenterOffset + PolygonIndex);
      
      // Advance to the next edge pair.
      Vertex1 = Vertex2;
      Vertex2 = Vertex3;
    }
  }
  return PolyFlag.ConvertWorkBuffers();
}

FPolyhedronMesh FPolyhedronOperations::Snub(const FPolyhedronMesh& Input) {
  // Leverage the Gyro operation in dual-space.
  FPolyhedronMesh Polyhedron1 = Dual(Input);
  FPolyhedronMesh Polyhedron2 = Gyro(Polyhedron1);
  return Dual(Polyhedron2);
}

FPolyhedronMesh FPolyhedronOperations::Meta(const FPolyhedronMesh& Input) {
  // Meta is a join-kis combo.
  FPolyhedronMesh Polyhedron1 = Join(Input);
  return Kis(Polyhedron1);
}

FPolyhedronMesh FPolyhedronOperations::Bevel(const FPolyhedronMesh& Input) {
  // Bevel is a ambo-truncate combo.
  FPolyhedronMesh Polyhedron1 = Ambo(Input);
  return Truncate(Polyhedron1);
}

