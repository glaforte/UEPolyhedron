// Copyright 2024 TabbyCoder
// Based on the Polyhédronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#include "PolyhedronStarter.h"

FPolyhedronMesh FPolyhedronStarter::CreateIcosahedron() {
  // Icosahedron from https://github.com/levskaya/polyhedronisme/blob/master/polyhedron.js

  FPolyhedronMesh Output;
  Output.Vertices.Reserve(12);
  Output.Vertices.Add({ 0, 0, 1.176 });
  Output.Vertices.Add({ 1.051, 0, 0.526 });
  Output.Vertices.Add({ 0.324, 1.0, 0.525 });
  Output.Vertices.Add({ -0.851, 0.618, 0.526 });
  Output.Vertices.Add({ -0.851, -0.618, 0.526 });
  Output.Vertices.Add({ 0.325, -1.0, 0.526 });
  Output.Vertices.Add({ 0.851, 0.618, -0.526 });
  Output.Vertices.Add({ 0.851, -0.618, -0.526 });
  Output.Vertices.Add({ -0.325, 1.0, -0.526 });
  Output.Vertices.Add({ -1.051, 0, -0.526 });
  Output.Vertices.Add({ -0.325, -1.0, -0.526 });
  Output.Vertices.Add({ 0, 0, -1.176 });

  Output.Polygons.Reserve(20);
  Output.Polygons.Add({ 0, 2, 1 });
  Output.Polygons.Add({ 0, 3, 2 });
  Output.Polygons.Add({ 0, 4, 3 });
  Output.Polygons.Add({ 0, 5, 4 });
  Output.Polygons.Add({ 0, 1, 5 });
  Output.Polygons.Add({ 1, 7, 5 });
  Output.Polygons.Add({ 1, 6, 7 });
  Output.Polygons.Add({ 1, 2, 6 });
  Output.Polygons.Add({ 2, 8, 6 });
  Output.Polygons.Add({ 2, 3, 8 });
  Output.Polygons.Add({ 3, 9, 8 });
  Output.Polygons.Add({ 3, 4, 9 });
  Output.Polygons.Add({ 4, 10, 9 });
  Output.Polygons.Add({ 4, 5, 10 });
  Output.Polygons.Add({ 5, 7, 10 });
  Output.Polygons.Add({ 6, 11, 7 });
  Output.Polygons.Add({ 6, 8, 11 });
  Output.Polygons.Add({ 7, 11, 10 });
  Output.Polygons.Add({ 8, 9, 11 });
  Output.Polygons.Add({ 9, 10, 11 });

  return Output;
}

FPolyhedronMesh FPolyhedronStarter::CreateOctahedron() {
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(6);
  Output.Vertices.Add({ 0, 0, 1.414 });
  Output.Vertices.Add({ 1.414, 0, 0 });
  Output.Vertices.Add({ 0, 1.414, 0 });
  Output.Vertices.Add({ -1.414, 0, 0 });
  Output.Vertices.Add({ 0, -1.414, 0 });
  Output.Vertices.Add({ 0, 0, -1.414 });

  Output.Polygons.Reserve(8);
  Output.Polygons.Add({ 0, 2, 1 });
  Output.Polygons.Add({ 0, 3, 2 });
  Output.Polygons.Add({ 0, 4, 3 });
  Output.Polygons.Add({ 0, 1, 4 });
  Output.Polygons.Add({ 1, 5, 4 });
  Output.Polygons.Add({ 1, 2, 5 });
  Output.Polygons.Add({ 2, 3, 5 });
  Output.Polygons.Add({ 3, 4, 5 });
  return Output;
}

FPolyhedronMesh FPolyhedronStarter::CreateDodecahedron() {
  // Dodecahedron from https://github.com/levskaya/polyhedronisme/blob/master/polyhedron.js

  FPolyhedronMesh Output;
  Output.Vertices.Reserve(20);
  Output.Vertices.Add({ 0, 0, 1.07047 });
  Output.Vertices.Add({ 0.713644, 0, 0.797878 });
  Output.Vertices.Add({ -0.356822, 0.618, 0.797878 });
  Output.Vertices.Add({ -0.356822, -0.618, 0.797878 });
  Output.Vertices.Add({ 0.797878, 0.618034, 0.356822 });
  Output.Vertices.Add({ 0.797878, -0.618, 0.356822 });
  Output.Vertices.Add({ -0.934172, 0.381966, 0.356822 });
  Output.Vertices.Add({ 0.136294, 1.0, 0.356822 });
  Output.Vertices.Add({ 0.136294, -1.0, 0.356822 });
  Output.Vertices.Add({ -0.934172, -0.381966, 0.356822 });
  Output.Vertices.Add({ 0.934172, 0.381966, -0.356822 });
  Output.Vertices.Add({ 0.934172, -0.381966, -0.356822 });
  Output.Vertices.Add({ -0.797878, 0.618, -0.356822 });
  Output.Vertices.Add({ -0.136294, 1.0, -0.356822 });
  Output.Vertices.Add({ -0.136294, -1.0, -0.356822 });
  Output.Vertices.Add({ -0.797878, -0.618034, -0.356822 });
  Output.Vertices.Add({ 0.356822, 0.618, -0.797878 });
  Output.Vertices.Add({ 0.356822, -0.618, -0.797878 });
  Output.Vertices.Add({ -0.713644, 0, -0.797878 });
  Output.Vertices.Add({ 0, 0, -1.07047 });

  Output.Polygons.Reserve(12);
  Output.Polygons.Add({ 0, 2, 7, 4, 1 });
  Output.Polygons.Add({ 0, 3, 9, 6, 2 });
  Output.Polygons.Add({ 0, 1, 5, 8, 3 });
  Output.Polygons.Add({ 1, 4, 10, 11, 5 });
  Output.Polygons.Add({ 2, 6, 12, 13, 7 });
  Output.Polygons.Add({ 3, 8, 14, 15, 9 });
  Output.Polygons.Add({ 4, 7, 13, 16, 10 });
  Output.Polygons.Add({ 5, 11, 17, 14, 8 });
  Output.Polygons.Add({ 6, 9, 15, 18, 12 });
  Output.Polygons.Add({ 10, 16, 19, 17, 11 });
  Output.Polygons.Add({ 12, 18, 19, 16, 13 });
  Output.Polygons.Add({ 14, 17, 19, 18, 15 });

  return Output;
}

FPolyhedronMesh FPolyhedronStarter::CreatePrism(int32 Sides) {
  if (Sides < 3) { 
    REPORT_ERROR("Cannot create a prism with a degree < 3");
    return FPolyhedronMesh();
  }

  // Fits the Prism on a sphere.
  const double Theta = (2.0 * UE_DOUBLE_PI) / static_cast<double>(Sides);
  const double HalfHeight = FMath::Sin(Theta / 2.0);

  FPolyhedronMesh Output;
  Output.Vertices.SetNum(Sides * 2);
  Output.Polygons.SetNum(Sides + 2);

  // Write the vertices.
  for (int32 VertexIndex = 0; VertexIndex < Sides; ++VertexIndex) {
    Output.Vertices[VertexIndex] = { -FMath::Cos(VertexIndex*Theta), -FMath::Sin(VertexIndex*Theta),  -HalfHeight };
    Output.Vertices[Sides + VertexIndex] = { -FMath::Cos(VertexIndex*Theta), -FMath::Sin(VertexIndex*Theta),  HalfHeight };
  }

  // Write the side polygons first.
  for (int32 VertexIndex = 0; VertexIndex < Sides; ++VertexIndex) {
    Output.Polygons[VertexIndex] = { VertexIndex, VertexIndex + Sides, (VertexIndex+1) % Sides + Sides, (VertexIndex+1) % Sides };
  }

  // Write the end polygons.
  for (int32 VertexIndex = 0; VertexIndex < Sides; ++VertexIndex) {
    Output.Polygons[Sides].VertexIndices.Add(VertexIndex);
    Output.Polygons[Sides + 1].VertexIndices.Add(2 * Sides - VertexIndex - 1); // Reverse Winding.
  }

  return Output;
}

FPolyhedronMesh FPolyhedronStarter::CreateTetrahedron() {
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(4);
  Output.Vertices.Add({ 1.0, 1.0, 1.0 });
  Output.Vertices.Add({ 1.0, -1.0, -1.0 });
  Output.Vertices.Add({ -1.0, 1.0, -1.0 });
  Output.Vertices.Add({ -1.0, -1.0, 1.0 });

  Output.Polygons.Reserve(4);
  Output.Polygons.Add({0, 2, 1});
  Output.Polygons.Add({0, 3, 2});
  Output.Polygons.Add({0, 1, 3});
  Output.Polygons.Add({1, 2, 3});
  
  return Output;
}

FPolyhedronMesh FPolyhedronStarter::CreateCube() {
  FPolyhedronMesh Output;
  Output.Vertices.Reserve(8);
  Output.Vertices.Add({ 0.707, 0.707, 0.707 });
  Output.Vertices.Add({ -0.707, 0.707, 0.707 });
  Output.Vertices.Add({ -0.707, -0.707, 0.707 });
  Output.Vertices.Add({ 0.707, -0.707, 0.707 });
  Output.Vertices.Add({ 0.707, -0.707, -0.707 });
  Output.Vertices.Add({ 0.707, 0.707, -0.707 });
  Output.Vertices.Add({ -0.707, 0.707, -0.707 });
  Output.Vertices.Add({ -0.707, -0.707, -0.707 });
  
  Output.Polygons.Reserve(6);
  Output.Polygons.Add({ 3, 2, 1, 0 });
  Output.Polygons.Add({ 3, 0, 5, 4 });
  Output.Polygons.Add({ 0, 1, 6, 5 });
  Output.Polygons.Add({ 1, 2, 7, 6 });
  Output.Polygons.Add({ 2, 3, 4, 7 });
  Output.Polygons.Add({ 5, 6, 7, 4 });
  return Output;
}
