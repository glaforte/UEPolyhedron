// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "Polyhedron.h"
#include "Helpers.h"

FPolyhedronPolygon::FPolyhedronPolygon()
  : MaterialIndex(0)
  , VertexIndices() {}

FPolyhedronPolygon::FPolyhedronPolygon(const TArray<int32>& Indices)
  : MaterialIndex(0)
  , VertexIndices(Indices) {}

FPolyhedronPolygon::FPolyhedronPolygon(std::initializer_list<int32> InitList)
  : MaterialIndex(0)
  , VertexIndices(InitList) {}

FPolyhedronMesh FPolyhedronTools::GenerateFromConwayPolyhedronNotation(const FString& ConwayPolyhedronNotation, float Scale) {
  REPORT_ERROR_RETURN_IF(ConwayPolyhedronNotation.Len() < 1, FPolyhedronMesh(), "Empty ConwayPolyhedronNotation makes no Polyhedron");

  // Some sample Conway notations that work:
  // I -> Icosahedron
  // dI -> Dodecahedron
  // tI -> Truncated Icosahedron or Soccer ball or G(1,1)
  // tktI -> Golf ball or G(3,3)

  // The last letter (and the first to be processed) is the start polyhedron.
  FPolyhedronMesh Polyhedron;
  auto NotationIterator = ConwayPolyhedronNotation.rbegin(), NotationIteratorEnd = ConwayPolyhedronNotation.rend();
  switch (*NotationIterator) {
  case 'I': Polyhedron = CreateIcosahedron(); break;
  default: REPORT_ERROR_RETURN_IF(ConwayPolyhedronNotation.Len() < 1, FPolyhedronMesh(), "Unknown Starter Volume.");
  }
  ++NotationIterator;

  // The subsequent letters are Conway operations to be done on the polyhedron.
  for (; NotationIterator != NotationIteratorEnd; ++NotationIterator) {
    switch (*NotationIterator) {
    case 'd': Polyhedron = ExecuteDualOperation(Polyhedron); break;
    case 'k': Polyhedron = ExecuteKisOperation(Polyhedron, 0, 0.1); break;
    case 't': Polyhedron = ExecuteTruncateOperation(Polyhedron); break;
    default: REPORT_ERROR_RETURN_IF(ConwayPolyhedronNotation.Len() < 1, FPolyhedronMesh(), "Unknown Operation: %s", *NotationIterator);
    }
  }

  Polyhedron = ScaleToSphere(Polyhedron, Scale);
  return Polyhedron;
}

FVector FPolyhedronTools::CalculateNormal(const FVector& Position1, const FVector& Position2, const FVector& Position3) {
  FVector Normal = FVector::CrossProduct(Position3 - Position1, Position2 - Position1);
  Normal.Normalize();
  return Normal;
}

TArray<FVector> FPolyhedronTools::GetPolygonCenters(const FPolyhedronMesh& Input) {
  TArray<FVector> Output;
  Output.Reserve(Input.Polygons.Num());

  // Calculate the center of polygon; essentially its average position.
  for (const FPolyhedronPolygon& Polygon : Input.Polygons) {
    FVector Center = FVector::ZeroVector;
    for (int32 VertexIndex : Polygon.VertexIndices) {
      Center += Input.Vertices[VertexIndex];
    }
    Output.Add(Center / Polygon.VertexIndices.Num());
  }

  return Output;
}

TArray<FVector> FPolyhedronTools::GetPolygonNormals(const FPolyhedronMesh& Input) {
  TArray<FVector> Output;
  Output.Reserve(Input.Polygons.Num());

  // Calculate the average normal for this polygon.
  // -- An average polygon normal feels like a weird concept.
  for (const FPolyhedronPolygon& Polygon : Input.Polygons) {
    FVector Normal = FVector::ZeroVector;
    const TArray<int32>& PolygonVertexIndices = Polygon.VertexIndices;

    // Fan-triangulate this polygon to process each triangle's normal.
    for (int32 PolygonVertexIndex = 2; PolygonVertexIndex < PolygonVertexIndices.Num(); ++PolygonVertexIndex) {
      Normal += CalculateNormal(Input.Vertices[PolygonVertexIndices[0]], Input.Vertices[PolygonVertexIndices[PolygonVertexIndex - 1]], Input.Vertices[PolygonVertexIndices[PolygonVertexIndex]]);
    }
    Output.Add(Normal.GetSafeNormal());
  }

  return Output;
}

FVector FPolyhedronTools::GetPolygonCenter(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon) {
  REPORT_ERROR_RETURN_IF(Polygon.VertexIndices.Num() < 3, FVector::ZeroVector, "No a complete polygon.");

  FVector Center = FVector::ZeroVector;
  for (int32 VertexIndex : Polygon.VertexIndices) {
    Center += Polyhedron.Vertices[VertexIndex];
  }
  return Center / Polygon.VertexIndices.Num();
}

FVector FPolyhedronTools::GetPolygonNormal(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon) {
  REPORT_ERROR_RETURN_IF(Polygon.VertexIndices.Num() < 3, FVector::ZeroVector, "No a complete polygon.");

  // Fan-triangulate this polygon to process each triangle's normal.
  FVector Normal = FVector::ZeroVector;
  const TArray<int32>& PolygonVertexIndices = Polygon.VertexIndices;
  for (int32 PolygonVertexIndex = 2; PolygonVertexIndex < PolygonVertexIndices.Num(); ++PolygonVertexIndex) {
    Normal += CalculateNormal(Polyhedron.Vertices[PolygonVertexIndices[0]], Polyhedron.Vertices[PolygonVertexIndices[PolygonVertexIndex - 1]], Polyhedron.Vertices[PolygonVertexIndices[PolygonVertexIndex]]);
  }
  return Normal.GetSafeNormal();
}

FPolyhedronMesh FPolyhedronTools::CreateIcosahedron() const {
  // Icosahedron from https://github.com/levskaya/polyhedronisme/blob/master/polyhedron.js

  FPolyhedronMesh Output;
  Output.Vertices.Reserve(12);
  Output.Vertices.Add({ 0, 0, 1.176 });
  Output.Vertices.Add({ 1.051, 0, 0.526 });
  Output.Vertices.Add({ 0.324, 1.0, 0.525 });
  Output.Vertices.Add({ -0.851, 0.618, 0.526 });
  Output.Vertices.Add({ -0.851, -0.618, 0.526 });
  Output.Vertices.Add({ 0.325, -1.0, 0.526 });
  Output.Vertices.Add({ 0.851, 0.618, -0.526 });
  Output.Vertices.Add({ 0.851, -0.618, -0.526 });
  Output.Vertices.Add({ -0.325, 1.0, -0.526 });
  Output.Vertices.Add({ -1.051, 0, -0.526 });
  Output.Vertices.Add({ -0.325, -1.0, -0.526 });
  Output.Vertices.Add({ 0, 0, -1.176 });

  Output.Polygons.Reserve(20);
  Output.Polygons.Add({ 0, 2, 1 });
  Output.Polygons.Add({ 0, 3, 2 });
  Output.Polygons.Add({ 0, 4, 3 });
  Output.Polygons.Add({ 0, 5, 4 });
  Output.Polygons.Add({ 0, 1, 5 });
  Output.Polygons.Add({ 1, 7, 5 });
  Output.Polygons.Add({ 1, 6, 7 });
  Output.Polygons.Add({ 1, 2, 6 });
  Output.Polygons.Add({ 2, 8, 6 });
  Output.Polygons.Add({ 2, 3, 8 });
  Output.Polygons.Add({ 3, 9, 8 });
  Output.Polygons.Add({ 3, 4, 9 });
  Output.Polygons.Add({ 4, 10, 9 });
  Output.Polygons.Add({ 4, 5, 10 });
  Output.Polygons.Add({ 5, 7, 10 });
  Output.Polygons.Add({ 6, 11, 7 });
  Output.Polygons.Add({ 6, 8, 11 });
  Output.Polygons.Add({ 7, 11, 10 });
  Output.Polygons.Add({ 8, 9, 11 });
  Output.Polygons.Add({ 9, 10, 11 });

  return Output;
}

FPolyhedronMesh FPolyhedronTools::ExecuteDualOperation(const FPolyhedronMesh& Input) const {
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
  int32 OutputVertexCount = Input.Polygons.Num(); // Input Polygon Count -> Output Vertex Count.
  int32 OutputPolygonCount = Input.Vertices.Num(); // Input Vertex Count -> Output Polygon Count.
  Output.Vertices.SetNum(OutputVertexCount);
  Output.Polygons.SetNum(OutputPolygonCount);

  // Compute the polygon centers: these become the vertices of the new mesh.
  TArray<FVector> Centers = GetPolygonCenters(Input);
  for (int32 CenterIndex = 0; CenterIndex < Centers.Num(); ++CenterIndex) {
    Output.Vertices[CenterIndex] = Centers[CenterIndex];
  }

  // Reverse the Extended Mesh' adjacency information to create Output edges.
  TArray<TArray<TPair<int32, int32>>> OutputPolygonEdgeLists;
  OutputPolygonEdgeLists.SetNum(OutputPolygonCount);
  FPolyhedronExtendedMesh ExtendedInput = FPolyhedronTools::ComputeEdgeDetails(Input);
  for (const FPolyhedronDirectedHalfEdge& HalfEdge: ExtendedInput.Edges) {
    // If this check hits, your mesh is not manifold.
    check(HalfEdge.PolygonIndexAcross >= 0);
    OutputPolygonEdgeLists[HalfEdge.VertexIndexFrom].Add(TPair<int32, int32>(HalfEdge.PolygonIndexAcross, HalfEdge.PolygonIndex));
  }

  // Process the output edges into output polygons with preserved winding.
  for (int32 OutputPolygonIndex = 0; OutputPolygonIndex < OutputPolygonCount; ++OutputPolygonIndex) {
    const TArray<TPair<int32, int32>>& OutputPolygonEdgeList = OutputPolygonEdgeLists[OutputPolygonIndex];
    int32 OutputPolygonVertexCount = OutputPolygonEdgeList.Num();
    Output.Polygons[OutputPolygonIndex].VertexIndices.Reserve(OutputPolygonVertexCount);

    // We can start with any of the half-edges.
    int32 OutputVertex1 = OutputPolygonEdgeList[0].Get<0>();
    int32 OutputVertex2 = OutputPolygonEdgeList[0].Get<1>();
    Output.Polygons[OutputPolygonIndex].VertexIndices.Add(OutputVertex1);
    Output.Polygons[OutputPolygonIndex].VertexIndices.Add(OutputVertex2);

    // Loop over the other half-edges, looking for the next vertex.
    bool bLoopCompleted = false;
    for (int32 Iterator = 1; Iterator < OutputPolygonVertexCount; ++Iterator) {
      int32 NextVertexIndex = -1;
      for (int32 EdgeIndex = 1; EdgeIndex < OutputPolygonVertexCount; ++EdgeIndex) {
        if (OutputPolygonEdgeList[EdgeIndex].Get<0>() == OutputVertex2) {
          NextVertexIndex = OutputPolygonEdgeList[EdgeIndex].Get<1>();
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

FPolyhedronMesh FPolyhedronTools::ExecuteKisOperation(const FPolyhedronMesh& Input, int32 SideFilter, double ApexOffset) const {
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

      Output.Vertices[NextApexId] = GetPolygonCenter(Input, Polygon) + ApexOffset * GetPolygonNormal(Input, Polygon);

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

FPolyhedronMesh FPolyhedronTools::ExecuteTruncateOperation(const FPolyhedronMesh& Input) const {
  // Truncate is a dual-kis-dual combo.
  FPolyhedronMesh Polyhedron1 = ExecuteDualOperation(Input);
  FPolyhedronMesh Polyhedron2 = ExecuteKisOperation(Polyhedron1, 0, 0.1);
  return ExecuteDualOperation(Polyhedron2);
}

FPolyhedronMesh FPolyhedronTools::ScaleToSphere(const FPolyhedronMesh& Input, double Radius) const {
  // Compute the bounding sphere of the polyhedron.
  FSphere BoundingSphere(Input.Vertices.GetData(), Input.Vertices.Num());
  const FVector& Centroid = BoundingSphere.Center; // should this always be the zero-vector?
  float ScaleFactor = Radius / BoundingSphere.W;

  // Rescale the polyhedron.
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(Input.Vertices.Num());
  for (const FVector& InputVertex : Input.Vertices) {
    FVector VertexOffset = InputVertex - Centroid;
#ifdef JUST_SCALE
    FVector OutputVertex = Centroid + (VertexOffset * ScaleFactor);
#else
    // Recenter at the origin and place back on the sphere.
    FVector OutputVertex = VertexOffset.GetSafeNormal() * Radius;
#endif    
    Output.Vertices.Add(OutputVertex);
  }

  Output.Polygons = Input.Polygons;
  return Output;
}

FPolyhedronExtendedMesh FPolyhedronTools::ComputeEdgeDetails(const FPolyhedronMesh& Input) {
  FPolyhedronExtendedMesh Output;

  // Copy the vertices and polygons.
  Output.Vertices = Input.Vertices;
  Output.Polygons = Input.Polygons;

  // Count the edges and allocate the edge list.
  int32 VertexTotal = Input.Vertices.Num();
  int32 EdgeTotal = 0;
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    EdgeTotal += Polygon.VertexIndices.Num();
  }
  Output.Edges.Reserve(EdgeTotal);

  // Process the polygon, record edges.
  TArray<TArray<TPair<int32, int32>>> EdgeWorkBuffer;
  EdgeWorkBuffer.SetNum(VertexTotal);
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];

    int32 Vertex1 = Polygon.VertexIndices.Last(); // Start with the last vertex.
    for (int32 VertexIndex : Polygon.VertexIndices) {
      int32 Vertex2 = VertexIndex;

      FPolyhedronDirectedHalfEdge HalfEdge;
      HalfEdge.PolygonIndex = PolygonIndex;
      HalfEdge.VertexIndexFrom = Vertex1;
      HalfEdge.VertexIndexTo = Vertex2;
      Output.Edges.Add(HalfEdge);

      EdgeWorkBuffer[HalfEdge.VertexIndexFrom].Add(TPair<int32, int32>(HalfEdge.VertexIndexTo, PolygonIndex));
      Vertex1 = Vertex2;
    }
  }

  // Process the edges and find adjacent polygons.
  for (FPolyhedronDirectedHalfEdge& HalfEdge : Output.Edges) {

    // Look through the edge work area for the reverse edge and its polygon index.
    HalfEdge.PolygonIndexAcross = -1;
    for (const TPair<int32, int32>& EdgesAroundVertex : EdgeWorkBuffer[HalfEdge.VertexIndexTo]) {
      if (EdgesAroundVertex.Get<0>() == HalfEdge.VertexIndexFrom) {
        HalfEdge.PolygonIndexAcross = EdgesAroundVertex.Get<1>();
        break;
      }
    }
  }

  return Output;
}

int32 FPolyhedronTools::GetPolygonAt(const FPolyhedronMesh& Input, const FVector& Location) {

  float BestDistanceSquared = 1e9f;
  int32 BestIndex = -1;
  for (int32 PolygonIndex = 0; PolygonIndex < Input.Polygons.Num(); ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];

    // Bounding box test.
    FBox PolygonBox;
    for (int32 VertexIndex : Polygon.VertexIndices) {
      PolygonBox += Input.Vertices[VertexIndex];
    }
    PolygonBox = PolygonBox.ExpandBy(FVector(1.0, 1.0, 1.0));

    if (!PolygonBox.IsInsideOrOn(Location)) continue;
    
    FVector PolygonCenter = PolygonBox.GetCenter();
    float ContestantDistanceSquared = FVector::DistSquared(PolygonCenter, Location);
    if (BestIndex >= 0 && ContestantDistanceSquared >= BestDistanceSquared) continue;

    BestIndex = PolygonIndex;
    BestDistanceSquared = ContestantDistanceSquared;
  }

  return BestIndex;
}

void FPolyhedronTools::ClearWorkBuffers() const {
  WorkVertexPositions.Empty();
  WorkVertexIndices.Empty();
  WorkPolygonFlags.Empty();
}

void FPolyhedronTools::AddWorkVertex(int64 VertexId, const FVector& Position) const {
  for (int64 VertexIndex = WorkVertexIndices.Num(); VertexIndex <= VertexId; ++VertexIndex) {
    WorkVertexIndices.Add(TPair<int64, int64>(VertexIndex, static_cast<int64>(0))); // Indexed at a later time?
    WorkVertexPositions.Add(TPair<int64, FVector>(VertexIndex, Position));
  }
  WorkVertexPositions[VertexId].Get<1>() = Position;
}

void FPolyhedronTools::AddWorkFlag(int64 FaceId, int64 VertexId1, int64 VertexId2) const {
  if (!WorkPolygonFlags.Contains(FaceId)) {
    WorkPolygonFlags.Add(FaceId, TMap<int64, int64>());
  }
  WorkPolygonFlags[FaceId].FindOrAdd(VertexId1) = VertexId2;
}

FPolyhedronMesh FPolyhedronTools::ConvertWorkBuffers() const {

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
    Output.Vertices.Add(WorkVertexPositions[Iterator.Get<0>()].Get<1>());
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
    OutputPolygon.Add(WorkVertexIndices[VertexIterator].Get<1>());
    VertexIterator = Face[VertexIterator];
    for (int32 BackstopCounter = 0; BackstopCounter < 1000; ++BackstopCounter) {
      if (VertexIterator == Vertex0) break;
      OutputPolygon.Add(WorkVertexIndices[VertexIterator].Get<1>());
      VertexIterator = Face[VertexIterator];
    }

    Output.Polygons.Add(OutputPolygon);
  }

  return Output;
}
