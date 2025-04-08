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

USTRUCT()
struct POLYHEDRON_API FPolyhedronTools {
  GENERATED_BODY()

public: // Conway Notation
  FPolyhedronMesh GenerateFromConwayPolyhedronNotation(const FString& ConwayPolyhedronNotation, float Scale = 100.0);

public: // Polygon Operations
  static FVector CalculateNormal(const FVector& Position1, const FVector& Position2, const FVector& Position3);
  static TArray<FVector> GetPolygonCenters(const FPolyhedronMesh& Input);
  static TArray<FVector> GetPolygonNormals(const FPolyhedronMesh& Input);
  static FVector GetPolygonCenter(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon);
  static FVector GetPolygonNormal(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon);

public: // Polyhedra Operations
  FPolyhedronMesh ExecuteDualOperation(const FPolyhedronMesh& Input) const;
  FPolyhedronMesh ExecuteKisOperation(const FPolyhedronMesh& Input, int32 SideFilter = 0, double ApexOffset = 0.1) const;
  FPolyhedronMesh ExecuteTruncateOperation(const FPolyhedronMesh& Input) const;
  FPolyhedronMesh ExecuteAmboOperation(const FPolyhedronMesh& Input) const;
  FPolyhedronMesh ExecuteJoinOperation(const FPolyhedronMesh& Input) const;
  FPolyhedronMesh ExecuteGyroOperation(const FPolyhedronMesh& Input) const;
  FPolyhedronMesh ScaleToSphere(const FPolyhedronMesh& Input, double Radius = 100.0) const;
  FPolyhedronMesh ProjectUntoSphere(const FPolyhedronMesh& Input, double Radius = 100.0) const;

public: // Polyhedra Extended Operations
  static FPolyhedronExtendedMesh ComputeEdgeDetails(const FPolyhedronMesh& Input);

public: // Locations
  static int32 GetPolygonAt(const FPolyhedronMesh& Input, const FVector& Location);

private: // Temporary Work Buffers.
  void ClearWorkBuffers() const;
  void AddWorkVertex(int64 VertexId, const FVector& Position) const;
  void AddWorkFlag(int64 FaceId, int64 VertexId1, int64 VertexId2) const;
  FPolyhedronMesh ConvertWorkBuffers() const;
  mutable TMap<int64, FVector> WorkVertexPositions;
  mutable TMap<int64, int64> WorkVertexIndices;
  mutable TMap<int64, TMap<int64, int64>> WorkPolygonFlags;
};
