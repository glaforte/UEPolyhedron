// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "PolyhedronOperations.generated.h"

/*
* Regroups the Polyhedron Operations functions.
*/
USTRUCT()
struct POLYHEDRON_API FPolyhedronOperations {
  GENERATED_BODY()

public: // Polyhedra Operations
  static FPolyhedronMesh Dual(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Kis(const FPolyhedronMesh& Input, int32 SideFilter = 0, double ApexOffset = 0.1);
  static FPolyhedronMesh Truncate(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Ambo(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Join(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Gyro(const FPolyhedronMesh& Input);
};
