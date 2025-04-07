// Copyright 2024 TabbyCoder

#include "PolyhedronComponent.h"
#include "Helpers.h"

UPolyhedronComponent::UPolyhedronComponent(const FObjectInitializer& ObjectInitializer)
  : UProceduralMeshComponent(ObjectInitializer) {

  SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
  SetCollisionResponseToAllChannels(ECR_Block);
}

void UPolyhedronComponent::SetPolyhedronMesh(const FPolyhedronMesh& Polyhedron, bool bEnableCollision) {

  // Count the number of materials.
  int32 MaterialCount = 0;
  for (const FPolyhedronPolygon& Polygon : Polyhedron.Polygons) {
    if (Polygon.MaterialIndex >= MaterialCount) {
      MaterialCount = Polygon.MaterialIndex + 1;
    }
  }

  // Restart the Procedural Mesh.
  ClearAllMeshSections();

  // Build each mesh section according to its material properties.
  for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex) {

    // Count the Polygon-Vertex pairs.
    int32 UniqueVertexTotal = 0;
    int32 TriangleTotal = 0;
    for (const FPolyhedronPolygon& Polygon : Polyhedron.Polygons) {
      if (Polygon.MaterialIndex != MaterialIndex) continue;
      int32 PolygonVertexCount = Polygon.VertexIndices.Num();
      if (PolygonVertexCount >= 3) {
        UniqueVertexTotal += PolygonVertexCount;
        TriangleTotal += PolygonVertexCount - 2; // Fan-triangulation
      }
    }

    // Skip material indices without polygons.
    if (TriangleTotal == 0) continue;

    // Allocate the mesh buffers.
    TArray<FVector> MeshPositions; MeshPositions.Reserve(UniqueVertexTotal);
    TArray<FVector> MeshNormals; MeshNormals.Reserve(UniqueVertexTotal);
    TArray<FVector2D> MeshUVs; MeshUVs.Reserve(UniqueVertexTotal);
    TArray<int32> MeshTriangles; MeshTriangles.Reserve(TriangleTotal * 3);

    // Iterate over the polygons
    for (const FPolyhedronPolygon& Polygon : Polyhedron.Polygons) {
      if (Polygon.MaterialIndex != MaterialIndex) continue;
      int32 PolygonVertexCount = Polygon.VertexIndices.Num();
      if (PolygonVertexCount < 3) continue;

      // Compute the vertex normal; this assumes planar polygons.
      FVector PolygonNormal = FPolyhedronTools::GetPolygonNormal(Polyhedron, Polygon);

      // Copy the vertex data into the final mesh arrays.
      int32 PolygonVertexOffset = MeshPositions.Num();
      for (const int32& VertexIndex : Polygon.VertexIndices) {
        MeshPositions.Add(Polyhedron.Vertices[VertexIndex]);
        MeshNormals.Add(PolygonNormal);
      }

      // Calculate the offset for each vertex position away from the center's normal.
      FVector UpAxis = FMath::Abs(FVector::ZAxisVector.Dot(PolygonNormal)) > 0.90 ? FVector::XAxisVector : FVector::ZAxisVector;
      FVector UpwardAxis = (UpAxis - UpAxis.ProjectOnToNormal(PolygonNormal)).GetUnsafeNormal();
      FVector SidewaysAxis = PolygonNormal.Cross(UpwardAxis).GetUnsafeNormal();
      FBox2D PolygonUVBounds;
      for (const int32& VertexIndex : Polygon.VertexIndices) {
        // We assume that the centroid of this polyhedron is always the origin.
        FVector CenterOffset = Polyhedron.Vertices[VertexIndex];
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

    REPORT_ERROR_IF(MeshPositions.Num() != UniqueVertexTotal, "Broken Algorithm -- Mismatched MeshPositions.Num()");
    REPORT_ERROR_IF(MeshNormals.Num() != UniqueVertexTotal, "Broken Algorithm -- Mismatched MeshNormals.Num()");
    REPORT_ERROR_IF(MeshUVs.Num() != UniqueVertexTotal, "Broken Algorithm -- Mismatched MeshUVs.Num()");
    REPORT_ERROR_IF(MeshTriangles.Num() != 3 * TriangleTotal, "Broken Algorithm -- Mismatched MeshTriangles.Num()");

    CreateMeshSection_LinearColor(MaterialIndex, MeshPositions, MeshTriangles, MeshNormals, MeshUVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), bEnableCollision, /*bSRGBConversion=*/false);
  }
}
