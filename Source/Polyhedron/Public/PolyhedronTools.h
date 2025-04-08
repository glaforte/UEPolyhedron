// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "PolyhedronTools.generated.h"

USTRUCT()
struct POLYHEDRON_API FPolyhedronTools {
  GENERATED_BODY()

public: // Conway Notation
  static FPolyhedronMesh GenerateFromConwayPolyhedronNotation(const FString& ConwayPolyhedronNotation, float Scale = 100.0);

public: // Polygon Operations
  static FVector CalculateNormal(const FVector& Position1, const FVector& Position2, const FVector& Position3);
  static TArray<FVector> GetPolygonCenters(const FPolyhedronMesh& Input);
  static TArray<FVector> GetPolygonNormals(const FPolyhedronMesh& Input);
  static FVector GetPolygonCenter(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon);
  static FVector GetPolygonNormal(const FPolyhedronMesh& Polyhedron, const FPolyhedronPolygon& Polygon);

public: // Polyhedra Operations
  static FPolyhedronMesh ScaleToSphere(const FPolyhedronMesh& Input, double Radius = 100.0);
  static FPolyhedronMesh ProjectUntoSphere(const FPolyhedronMesh& Input, double Radius = 100.0);

public: // Polyhedra Extended Operations
  static FPolyhedronExtendedMesh ComputeEdgeDetails(const FPolyhedronMesh& Input);

public: // Locations
  static int32 GetPolygonAt(const FPolyhedronMesh& Input, const FVector& Location);
};
