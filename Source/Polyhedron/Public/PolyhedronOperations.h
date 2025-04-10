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

public: // Edge Factor 1
  static FPolyhedronMesh Dual(const FPolyhedronMesh& Input);

public: // Edge Factor 2
  static FPolyhedronMesh Ambo(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Join(const FPolyhedronMesh& Input);

public: // Edge Factor 3
  static FPolyhedronMesh Kis(const FPolyhedronMesh& Input, int32 SideFilter = 0, double ApexOffset = 0.1);
  static FPolyhedronMesh Needle(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Zip(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Truncate(const FPolyhedronMesh& Input);

public: // Edge Factor 4
  static FPolyhedronMesh Chamfer(const FPolyhedronMesh& Input, double Offset = 0.1);
  static FPolyhedronMesh Expand(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Ortho(const FPolyhedronMesh& Input);

public: // Edge Factor 5
  static FPolyhedronMesh Gyro(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Snub(const FPolyhedronMesh& Input);

public: // Edge Factor 6
  static FPolyhedronMesh Meta(const FPolyhedronMesh& Input);
  static FPolyhedronMesh Bevel(const FPolyhedronMesh& Input);
};
