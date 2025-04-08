// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "PolyhedronTools.h"
#include "PolyhedronStarter.h"
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
      Argument = *NotationIterator - TEXT('0');
      continue;
    }

    // Start with a Polyhedron seed.
    if (!PolyhedronStarted) {
      switch (*NotationIterator) {
      case 'C': Polyhedron = FPolyhedronStarter::Cube(); break;
      case 'D': Polyhedron = FPolyhedronStarter::Dodecahedron(); break;
      case 'I': Polyhedron = FPolyhedronStarter::Icosahedron(); break;
      case 'O': Polyhedron = FPolyhedronStarter::Octahedron(); break;
      case 'P': Polyhedron = FPolyhedronStarter::Prism(Argument); break;
      case 'T': Polyhedron = FPolyhedronStarter::Tetrahedron(); break;
      default: REPORT_ERROR("Unknown Starter Volume: %c", *NotationIterator); return FPolyhedronMesh();
      }
      PolyhedronStarted = true;
      Argument = 0;
      continue;
    }

    // The subsequent letters are Conway operations to be done on the polyhedron.
    switch (*NotationIterator) {
    case 'a': Polyhedron = FPolyhedronOperations::Ambo(Polyhedron); break;
    case 'd': Polyhedron = FPolyhedronOperations::Dual(Polyhedron); break;
    case 'g': Polyhedron = FPolyhedronOperations::Gyro(Polyhedron); break;
    case 'j': Polyhedron = FPolyhedronOperations::Join(Polyhedron); break;
    case 'k': Polyhedron = FPolyhedronOperations::Kis(Polyhedron, 0, 0.1); break;
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
