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

FPolyhedronDirectedHalfEdge* FPolyhedronExtendedMesh::FindHalfEdge(int32 Vertex1, int32 Vertex2) {
  int32 Vertex1Iterator = VertexHalfEdgeOffsets[Vertex1];
  int32 Vertex1IteratorEnd = VertexHalfEdgeOffsets[Vertex1 + 1];
  for (; Vertex1Iterator < Vertex1IteratorEnd; ++Vertex1Iterator) {
    if (VertexHalfEdgeIndices[Vertex1Iterator].Get<0>() == Vertex2) {
      int32 HalfEdgeIndex = VertexHalfEdgeIndices[Vertex1Iterator].Get<1>();
      return &PolygonHalfEdges[HalfEdgeIndex];
    }
  }
  return nullptr;
}