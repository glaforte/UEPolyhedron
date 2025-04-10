// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "PolyhedronTools.h"
#include "PolyhedronSeeds.h"
#include "PolyhedronOperations.h"
#include "Helpers.h"

FPolyhedronMesh FPolyhedronTools::GenerateFromConwayPolyhedronNotation(const FString& ConwayPolyhedronNotation, float Scale) {
  REPORT_ERROR_RETURN_IF(ConwayPolyhedronNotation.Len() < 1, FPolyhedronMesh(), "Empty ConwayPolyhedronNotation makes no Polyhedron");

  // Some sample Conway notations that work:
  // I -> Icosahedron
  // dI -> Dodecahedron
  // tI -> Truncated Icosahedron or Soccer ball or G(1,1)
  // tktI -> Golf ball or G(3,3)

  // The last letter (and the first to be processed) is the start polyhedron.
  bool PolyhedronStarted = false;
  int32 Argument = 0;
  FPolyhedronMesh Polyhedron;
  auto NotationIterator = ConwayPolyhedronNotation.rbegin(), NotationIteratorEnd = ConwayPolyhedronNotation.rend();
  for (; NotationIterator != NotationIteratorEnd; ++NotationIterator) {
    // Parse any integers as an argument for the subsequent function.
    if (*NotationIterator >= '0' && *NotationIterator <= '9') {
      Argument = (Argument * 10) + (*NotationIterator - TEXT('0'));
      continue;
    }

    // Start with a Polyhedron seed.
    if (!PolyhedronStarted) {
      switch (*NotationIterator) {
      case 'A': Polyhedron = FPolyhedronSeeds::Antiprism(Argument); break;
      case 'C': Polyhedron = FPolyhedronSeeds::Cube(); break;
      case 'D': Polyhedron = FPolyhedronSeeds::Dodecahedron(); break;
      case 'I': Polyhedron = FPolyhedronSeeds::Icosahedron(); break;
      case 'O': Polyhedron = FPolyhedronSeeds::Octahedron(); break;
      case 'P': Polyhedron = FPolyhedronSeeds::Prism(Argument); break;
      case 'T': Polyhedron = FPolyhedronSeeds::Tetrahedron(); break;
      case 'Y': Polyhedron = FPolyhedronSeeds::Pyramid(Argument); break;
      default: REPORT_ERROR("Unknown Starter Volume: %c", *NotationIterator); return FPolyhedronMesh();
      }
      PolyhedronStarted = true;
      Argument = 0;
      continue;
    }

    // The subsequent letters are Conway operations to be done on the polyhedron.
    switch (*NotationIterator) {
    case 'a': Polyhedron = FPolyhedronOperations::Ambo(Polyhedron); break;
    case 'b': Polyhedron = FPolyhedronOperations::Bevel(Polyhedron); break;
    case 'c': Polyhedron = FPolyhedronOperations::Chamfer(Polyhedron); break;
    case 'd': Polyhedron = FPolyhedronOperations::Dual(Polyhedron); break;
    case 'e': Polyhedron = FPolyhedronOperations::Expand(Polyhedron); break;
    case 'g': Polyhedron = FPolyhedronOperations::Gyro(Polyhedron); break;
    case 'j': Polyhedron = FPolyhedronOperations::Join(Polyhedron); break;
    case 'k': Polyhedron = FPolyhedronOperations::Kis(Polyhedron, 0, 0.1); break;
    case 'm': Polyhedron = FPolyhedronOperations::Meta(Polyhedron); break;
    case 'n': Polyhedron = FPolyhedronOperations::Needle(Polyhedron); break;
    case 'o': Polyhedron = FPolyhedronOperations::Ortho(Polyhedron); break;
    case 's': Polyhedron = FPolyhedronOperations::Snub(Polyhedron); break;
    case 't': Polyhedron = FPolyhedronOperations::Truncate(Polyhedron); break;
    default: REPORT_ERROR("Unknown Polyhedron Operation: %c", *NotationIterator); return FPolyhedronMesh();
    }
    Argument = 0;
  }
  
  Polyhedron = ScaleToSphere(Polyhedron, Scale);
  return Polyhedron;
}

FVector FPolyhedronTools::CalculateNormal(const FVector& Position1, const FVector& Position2, const FVector& Position3) {
  FVector Normal = FVector::CrossProduct(Position3 - Position1, Position2 - Position1);
  Normal.Normalize(1e-12); // UE_SMALL_NUMBER is 1e-8f, try smaller since we are using doubles.
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

FPolyhedronMesh FPolyhedronTools::ScaleToSphere(const FPolyhedronMesh& Input, double Radius) {
  // Compute the current spherical radius of the polyhedron.
  // Assume that all Polyhedron have the origin as their center.
  double FurthestVertexDistanceSquared = 0.0;
  for (const FVector& VertexPosition : Input.Vertices) {
    double VertexDistanceSquared = VertexPosition.Dot(VertexPosition);
    if (VertexDistanceSquared > FurthestVertexDistanceSquared) {
      FurthestVertexDistanceSquared = VertexDistanceSquared;
    }
  }
  FSphere BoundingSphere(Input.Vertices.GetData(), Input.Vertices.Num());
  const FVector& Centroid = BoundingSphere.Center; // should this always be the zero-vector?
  float ScaleFactor = Radius / FMath::Sqrt(FurthestVertexDistanceSquared);

  // Rescale the polyhedron.
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(Input.Vertices.Num());
  for (const FVector& InputVertex : Input.Vertices) {
    Output.Vertices.Add(InputVertex * ScaleFactor);
  }

  Output.Polygons = Input.Polygons;
  return Output;
}

FPolyhedronMesh FPolyhedronTools::ProjectUntoSphere(const FPolyhedronMesh& Input, double Radius) {
  // Assume that all Polyhedron have the origin as their center.
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(Input.Vertices.Num());
  for (const FVector& InputVertex : Input.Vertices) {
    // Recenter at the origin and place back on the sphere.
    Output.Vertices.Add(InputVertex.GetSafeNormal() * Radius);
  }

  Output.Polygons = Input.Polygons;
  return Output;
}

FPolyhedronExtendedMesh FPolyhedronTools::ComputeEdgeDetails(const FPolyhedronMesh& Input) {
  FPolyhedronExtendedMesh Output;

  // Copy the vertices and polygons.
  Output.Vertices = Input.Vertices;
  Output.Polygons = Input.Polygons;
  int32 VertexTotal = Output.Vertices.Num();
  if (VertexTotal < 1) return Output; // Empty mesh.
  int32 PolygonTotal = Output.Polygons.Num();
  if (PolygonTotal < 1) return Output; // Empty mesh.

  // Count the number of edges per vertex and per polygons, then cumulate them.
  int32 PolygonHalfEdgeTotal = 0;
  Output.VertexHalfEdgeOffsets.SetNumZeroed(VertexTotal + 1);
  Output.PolygonHalfEdgeOffsets.SetNum(PolygonTotal + 1);
  for (int32 PolygonIndex = 0; PolygonIndex < PolygonTotal; ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];
    for (int32 VertexIndex : Polygon.VertexIndices) {
      ++Output.VertexHalfEdgeOffsets[VertexIndex];
    }
    Output.PolygonHalfEdgeOffsets[PolygonIndex] = PolygonHalfEdgeTotal;
    PolygonHalfEdgeTotal += Polygon.VertexIndices.Num();
  }
  Output.PolygonHalfEdgeOffsets[PolygonTotal] = PolygonHalfEdgeTotal;
  int32 VertexHalfEdgeTotal = 0;
  for (int32 VertexIndex = 0; VertexIndex < VertexTotal; ++VertexIndex) {
    int32 VertexHalfEdgeOffset = VertexHalfEdgeTotal;
    VertexHalfEdgeTotal += Output.VertexHalfEdgeOffsets[VertexIndex];
    Output.VertexHalfEdgeOffsets[VertexIndex] = VertexHalfEdgeOffset;
  }
  Output.VertexHalfEdgeOffsets[VertexTotal] = VertexHalfEdgeTotal;

  // This is the same information, counted with two different methods.
  check(PolygonHalfEdgeTotal == VertexHalfEdgeTotal);
  int32 HalfEdgeTotal = PolygonHalfEdgeTotal;

  // Allocate the edge list and the per-vertex edge map.
  Output.VertexHalfEdgeIndices.SetNum(HalfEdgeTotal);
  Output.PolygonHalfEdges.SetNum(HalfEdgeTotal);

  // Unfortunately, we have to manually initialize the vertex half-edge map to "non-initialized".
  for (int32 HalfEdgeIndex = 0; HalfEdgeIndex < HalfEdgeTotal; ++HalfEdgeIndex) {
    Output.VertexHalfEdgeIndices[HalfEdgeIndex].Get<0>() = -1;
  }

  // Process the polygons, record half-edges in both per-vertex and per-polygon maps.
  int32 PolygonHalfEdgeIndex = 0;
  for (int32 PolygonIndex = 0; PolygonIndex < PolygonTotal; ++PolygonIndex) {
    const FPolyhedronPolygon& Polygon = Input.Polygons[PolygonIndex];

    int32 Vertex1 = Polygon.VertexIndices.Last(); // Start with the last vertex.
    for (int32 PolygonEdgeIndex = 0; PolygonEdgeIndex < Polygon.VertexIndices.Num(); ++PolygonEdgeIndex) {
      int32 Vertex2 = Polygon.VertexIndices[PolygonEdgeIndex];

      int32 HalfEdgeIndex = PolygonHalfEdgeIndex++;
      FPolyhedronDirectedHalfEdge& HalfEdge = Output.PolygonHalfEdges[HalfEdgeIndex];
      HalfEdge.PolygonIndex = PolygonIndex;
      HalfEdge.PolygonIndexAcross = -1;
      HalfEdge.VertexIndexFrom = Vertex1;
      HalfEdge.VertexIndexTo = Vertex2;

      // Record this half-edge index in the per-vertex map as well.
      bool bWrittenToVertexMap = false;
      for (int32 VertexHalfEdgeOffset = Output.VertexHalfEdgeOffsets[Vertex1], VertexHalfEdgeOffsetNext = Output.VertexHalfEdgeOffsets[Vertex1 + 1]; VertexHalfEdgeOffset < VertexHalfEdgeOffsetNext; ++VertexHalfEdgeOffset) {
        if (Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<0>() == -1) {
          Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<0>() = Vertex2;
          Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<1>() = HalfEdgeIndex;
          bWrittenToVertexMap = true;
          break;
        }
      }
      check (bWrittenToVertexMap); // your mesh is not manifold if this check triggers.

      // Check if the reverse edge has been processed.
      for (int32 VertexHalfEdgeOffset = Output.VertexHalfEdgeOffsets[Vertex2], VertexHalfEdgeOffsetNext = Output.VertexHalfEdgeOffsets[Vertex2 + 1]; VertexHalfEdgeOffset < VertexHalfEdgeOffsetNext; ++VertexHalfEdgeOffset) {
        if (Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<0>() == Vertex1) {
          int32 ReverseHalfEdgeIndex = Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<1>();
          HalfEdge.PolygonIndexAcross = Output.PolygonHalfEdges[ReverseHalfEdgeIndex].PolygonIndex;
          check(Output.PolygonHalfEdges[ReverseHalfEdgeIndex].PolygonIndexAcross == -1); // your mesh is not manifold if this check triggers.
          Output.PolygonHalfEdges[ReverseHalfEdgeIndex].PolygonIndexAcross = HalfEdge.PolygonIndex;
          break;
        } else if (Output.VertexHalfEdgeIndices[VertexHalfEdgeOffset].Get<0>() == -1) {
          // The reverse edge may be written at a later time; when it is processed: it will write the PolygonIndexAcross.
          break;
        }
      }

      // Advance to the next edge.
      Vertex1 = Vertex2;
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
