// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "Polyhedron.h"

FPolyhedronPolygon::FPolyhedronPolygon()
  : MaterialIndex(0)
  , VertexIndices() {}

FPolyhedronPolygon::FPolyhedronPolygon(const TArray<int32>& Indices)
  : MaterialIndex(0)
  , VertexIndices(Indices) {}

FPolyhedronPolygon::FPolyhedronPolygon(std::initializer_list<int32> InitList)
  : MaterialIndex(0)
  , VertexIndices(InitList) {}
