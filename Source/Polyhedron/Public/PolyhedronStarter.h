// Copyright 2024 TabbyCoder
// Based on the Polyh�dronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "PolyhedronStarter.generated.h"

/*
* Regroups the Polyhedron Starter creation functions.
*/
USTRUCT()
struct POLYHEDRON_API FPolyhedronStarter {
  GENERATED_BODY()

public: // Polyhedron Creation Functions
  static FPolyhedronMesh Icosahedron();
  static FPolyhedronMesh Dodecahedron();
  static FPolyhedronMesh Octahedron();
  static FPolyhedronMesh Prism(int32 Sides);
  static FPolyhedronMesh Tetrahedron();
  static FPolyhedronMesh Cube();
};

