// Copyright 2024 TabbyCoder

#include "PolyhedronPolygonComponent.h"
#include "Helpers.h"

UPolyhedronPolygonComponent::UPolyhedronPolygonComponent(const FObjectInitializer& ObjectInitializer)
  : UProceduralMeshComponent(ObjectInitializer) {}

void UPolyhedronPolygonComponent::Reset(int32 ComponentIndex) {
  if (ComponentIndex < 0) {
    // Restart the Procedural Mesh...
    ClearAllMeshSections();
  } else {
    REPORT_ERROR_IF(ComponentIndex >= GetNumSections(), "Invalid ComponentIndex");
    ClearMeshSection(ComponentIndex);
  }
}

void UPolyhedronPolygonComponent::SetPolyhedronPolygon(int32 ComponentIndex, const FPolyhedronMesh& PolyhedronMesh, int32 PolygonIndex, float Offset) {
  TArray<int32> PolygonIndices;
  PolygonIndices.Add(PolygonIndex);
  SetPolyhedronPolygon(ComponentIndex, PolyhedronMesh, PolygonIndices, Offset);
}

void UPolyhedronPolygonComponent::SetPolyhedronPolygon(int32 ComponentIndex, const FPolyhedronMesh& PolyhedronMesh, const TArray<int32>& PolygonIndices, float Offset) {
  REPORT_ERROR_IF(PolygonIndices.Num() == 0, "No PolygonIndices provided");

  // Prepare the vertex arrays.
  int VertexTotal = 0, TriangleTotal = 0;
  for (int32 PolygonIndex : PolygonIndices) {
    const FPolyhedronPolygon& Polygon = PolyhedronMesh.Polygons[PolygonIndex];
    int32 PolygonVertexCount = Polygon.VertexIndices.Num();
    VertexTotal += PolygonVertexCount;

    REPORT_ERROR_IF(PolygonVertexCount < 3, "Broken polygon");
    TriangleTotal += PolygonVertexCount - 2; // Fan-triangulation
  }

  // Allocate the mesh buffers.
  TArray<FVector> MeshPositions; MeshPositions.Reserve(VertexTotal);
  TArray<FVector> MeshNormals; MeshNormals.Reserve(VertexTotal);
  TArray<FVector2D> MeshUVs; MeshUVs.Reserve(VertexTotal);
  TArray<int32> MeshTriangles; MeshTriangles.Reserve(TriangleTotal * 3);

  // Fill the vertex arrays.
  for (int32 PolygonIndex : PolygonIndices) {
    const FPolyhedronPolygon& Polygon = PolyhedronMesh.Polygons[PolygonIndex];
    int32 PolygonVertexCount = Polygon.VertexIndices.Num();

    // Compute the vertex normal; this assumes planar polygons.
    FVector PolygonNormal = FPolyhedronTools::GetPolygonNormal(PolyhedronMesh, Polygon);

    // Copy the vertex data into the final mesh arrays.
    int32 PolygonVertexOffset = MeshPositions.Num();
    for (const int32& VertexIndex : Polygon.VertexIndices) {
      // Offset the positions slightly to avoid Z-fighting.
      MeshPositions.Add(PolyhedronMesh.Vertices[VertexIndex] + PolygonNormal * Offset);
      MeshNormals.Add(PolygonNormal);
    }

    // Calculate the offset for each vertex position away from the center's normal.
    FVector UpAxis = FMath::Abs(FVector::ZAxisVector.Dot(PolygonNormal)) > 0.90 ? FVector::XAxisVector : FVector::ZAxisVector;
    FVector UpwardAxis = (UpAxis - UpAxis.ProjectOnToNormal(PolygonNormal)).GetUnsafeNormal();
    FVector SidewaysAxis = PolygonNormal.Cross(UpwardAxis).GetUnsafeNormal();
    FBox2D PolygonUVBounds;
    for (const int32& VertexIndex : Polygon.VertexIndices) {
      // We assume that the centroid of this polyhedron is always the origin.
      FVector CenterOffset = PolyhedronMesh.Vertices[VertexIndex];
      CenterOffset -= CenterOffset.ProjectOnToNormal(PolygonNormal);
      FVector2D PlanarOffset(CenterOffset.Dot(SidewaysAxis), CenterOffset.Dot(UpwardAxis));
      PolygonUVBounds += PlanarOffset;
      MeshUVs.Add(PlanarOffset);
    }

    // Rescale the UV in the [0,1] range -- Should we add a property for texel density instead?
    FVector2D PolygonUVCenter, PolygonUVExtent;
    PolygonUVBounds.GetCenterAndExtents(PolygonUVCenter, PolygonUVExtent);
    FVector2D PolygonUVBottomLeft = PolygonUVCenter - PolygonUVExtent;
    FVector2D PolygonUVTopRight = PolygonUVCenter + PolygonUVExtent;
    float PolygonUVScaleFactor = 0.5 / FMath::Max(1e-6, PolygonUVExtent.GetMax());
    for (int32 PolygonVertexIndex = 0; PolygonVertexIndex < Polygon.VertexIndices.Num(); ++PolygonVertexIndex) {
      FVector2D& MeshUV = MeshUVs[PolygonVertexOffset + PolygonVertexIndex];
      MeshUV = PolygonUVScaleFactor * (MeshUV - PolygonUVBottomLeft);
    }

    // Fan-triangulate the polygon into the mesh arrays.
    for (int32 PolygonVertexIndex = 2; PolygonVertexIndex < PolygonVertexCount; ++PolygonVertexIndex) {
      MeshTriangles.Add(PolygonVertexOffset);
      MeshTriangles.Add(PolygonVertexOffset + PolygonVertexIndex - 1);
      MeshTriangles.Add(PolygonVertexOffset + PolygonVertexIndex);
    }
  }

  REPORT_ERROR_IF(MeshPositions.Num() != VertexTotal, "Broken Algorithm -- Mismatched MeshPositions.Num()");
  REPORT_ERROR_IF(MeshNormals.Num() != VertexTotal, "Broken Algorithm -- Mismatched MeshNormals.Num()");
  REPORT_ERROR_IF(MeshUVs.Num() != VertexTotal, "Broken Algorithm -- Mismatched MeshUVs.Num()");
  REPORT_ERROR_IF(MeshTriangles.Num() != 3 * TriangleTotal, "Broken Algorithm -- Mismatched MeshTriangles.Num()");

  CreateMeshSection_LinearColor(ComponentIndex, MeshPositions, MeshTriangles, MeshNormals, MeshUVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), /*bCreateCollision=*/false, /*bSRGBConversion=*/false);
}
