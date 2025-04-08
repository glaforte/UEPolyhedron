// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.generated.h"

USTRUCT()
struct POLYHEDRON_API FPolyhedronPolygon {
  GENERATED_BODY()

public:
  FPolyhedronPolygon();
  FPolyhedronPolygon(const TArray<int32>& Indices);
  FPolyhedronPolygon(std::initializer_list<int32> InitList);

public:
  int32 MaterialIndex = 0;
  TArray<int32> VertexIndices;
};

USTRUCT()
struct POLYHEDRON_API FPolyhedronMesh {
  GENERATED_BODY()

public:
  TArray<FVector> Vertices;
  TArray<FPolyhedronPolygon> Polygons;

public:
  int32 GetVertexCount() const { return Vertices.Num(); }
  int32 GetPolygonCount() const { return Polygons.Num(); }
};

/**
 * This structure represent a directed half-edge.
 */
USTRUCT()
struct POLYHEDRON_API FPolyhedronDirectedHalfEdge {
  GENERATED_BODY()

public:
  int32 VertexIndexFrom, VertexIndexTo;
  int32 PolygonIndex, PolygonIndexAcross;
};

USTRUCT()
struct POLYHEDRON_API FPolyhedronExtendedMesh : public FPolyhedronMesh {
  GENERATED_BODY()

public:
  TArray<FPolyhedronDirectedHalfEdge> Edges;
};
