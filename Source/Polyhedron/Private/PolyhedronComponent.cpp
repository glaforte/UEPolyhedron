// Copyright 2024 TabbyCoder

#include "PolyhedronComponent.h"
#include "Helpers.h"

namespace {
  FVector ConvertToSphericalCoordinates(const FVector& CartesianPoint) {
    double X = CartesianPoint.X;
    double Y = CartesianPoint.Y;
    double Z = CartesianPoint.Z;

    // Calculate radius
    double Radius = FMath::Sqrt(X * X + Y * Y + Z * Z);

    // Calculate theta or azimuth or longitude
    double Theta = FMath::Atan2(Y, X);

    // Calculate phi or inclination or latitude
    double Phi = (Radius > 0.0f) ? FMath::Asin(Z / Radius) : 0.0f;

    return FVector(Radius, 1.0 - (Theta + UE_DOUBLE_PI) / UE_DOUBLE_TWO_PI, 1.0 - (Phi + UE_DOUBLE_HALF_PI) / UE_DOUBLE_PI);
  }

  enum class EPolyhedronCubicFace : uint8 {
    PositiveX, PositiveY, PositiveZ, NegativeX, NegativeY, NegativeZ
  };

  EPolyhedronCubicFace ChooseFaceForCubicProjection(const FVector& Normal) {
    double AX = FMath::Abs(Normal.X);
    double AY = FMath::Abs(Normal.Y);
    double AZ = FMath::Abs(Normal.Z);
    if (AX > AY && AX > AZ) {
      return Normal.X > 0 ? EPolyhedronCubicFace::PositiveX : EPolyhedronCubicFace::NegativeX;
    } else if (AY > AX && AY > AZ) {
      return Normal.Y > 0 ? EPolyhedronCubicFace::PositiveY : EPolyhedronCubicFace::NegativeY;
    } else {
      return Normal.Z > 0 ? EPolyhedronCubicFace::PositiveZ : EPolyhedronCubicFace::NegativeZ;
    }
  }

  FVector2D ProjectOntoCube(const FVector& Normal, EPolyhedronCubicFace Pin) {
    switch (Pin) {
    case EPolyhedronCubicFace::PositiveX: return FVector2D(Normal.Y, -Normal.Z); break;
    case EPolyhedronCubicFace::NegativeX: return FVector2D(-Normal.Y, Normal.Z); break;
    case EPolyhedronCubicFace::PositiveY: return FVector2D(Normal.X, -Normal.Z); break;
    case EPolyhedronCubicFace::NegativeY: return FVector2D(-Normal.X, Normal.Z); break;
    case EPolyhedronCubicFace::PositiveZ: return FVector2D(Normal.X, -Normal.Y); break;
    default:
    case EPolyhedronCubicFace::NegativeZ: return FVector2D(-Normal.X, Normal.Y); break;
    }
  }
}

UPolyhedronComponent::UPolyhedronComponent(const FObjectInitializer& ObjectInitializer)
  : UProceduralMeshComponent(ObjectInitializer) {

  SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
  SetCollisionResponseToAllChannels(ECR_Block);
}

void UPolyhedronComponent::SetPolyhedronMesh(const FPolyhedronMesh& Polyhedron, bool bEnableCollision, EPolyhedronUVGeneration UVGeneration) {

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
      if (UVGeneration == EPolyhedronUVGeneration::Cellular) {
        // Use the PolygonNormal to generate a 2D basis for the local UV coordinates.
        FVector UpAxis = FMath::Abs(FVector::ZAxisVector.Dot(PolygonNormal)) > 0.90 ? FVector::XAxisVector : FVector::ZAxisVector;
        FVector UpwardAxis = (UpAxis - UpAxis.ProjectOnToNormal(PolygonNormal)).GetUnsafeNormal();
        FVector SidewaysAxis = PolygonNormal.Cross(UpwardAxis).GetUnsafeNormal();

        // Compute the center of the polygon, which will be (0.5, 0.5), and the 2D bounding box of the polygon.
        // The center of the polygon produces a PlanarOffset of (0, 0).
        FVector PolygonCenter = FPolyhedronTools::GetPolygonCenter(Polyhedron, Polygon);
        FBox2D PolygonUVBounds;

        for (const int32& VertexIndex : Polygon.VertexIndices) {
          FVector CenterOffset = Polyhedron.Vertices[VertexIndex] - PolygonCenter;
          CenterOffset -= CenterOffset.ProjectOnToNormal(PolygonNormal);
          FVector2D PlanarOffset(CenterOffset.Dot(SidewaysAxis), CenterOffset.Dot(UpwardAxis));
          PolygonUVBounds += PlanarOffset;
          MeshUVs.Add(PlanarOffset); // Record this value, but it is not a texture coordinate.
        }

        // Rescale the texture coordinates in the [0,1] range, around the center.
        //--------
        // Note that the UV Bounds should always include (0, 0), which we want to map to (0.5, 0.5).
        // Retrieve the largest distance in Min/Max and in U/V.
        double UVBoundMaxDistance = FMath::Max(-PolygonUVBounds.Min.GetMin(), PolygonUVBounds.Max.GetMax());
        float PolygonUVScaleFactor = 0.5 / UVBoundMaxDistance;
        for (int32 PolygonVertexIndex = 0; PolygonVertexIndex < Polygon.VertexIndices.Num(); ++PolygonVertexIndex) {
          FVector2D& MeshUV = MeshUVs[PolygonVertexOffset + PolygonVertexIndex];
          MeshUV = FVector2D(0.5, 0.5) + PolygonUVScaleFactor * MeshUV;
        }
      } else if (UVGeneration == EPolyhedronUVGeneration::Spherical) {
        // To fix the wrapping problem, pin on the first vertex of the polygon.
        FVector SphereProjectedPin = ConvertToSphericalCoordinates(Polyhedron.Vertices[Polygon.VertexIndices[0]]);
        double PinU = SphereProjectedPin.Y, PinV = SphereProjectedPin.Z;
        MeshUVs.Add(FVector2D(PinU, PinV));

        for (int32 PolygonVertexIndex = 1; PolygonVertexIndex < PolygonVertexCount; ++PolygonVertexIndex) {
          // Project each coordinate into spherical coordinates.
          int32 VertexIndex = Polygon.VertexIndices[PolygonVertexIndex];
          FVector SphereProjectedVertex = ConvertToSphericalCoordinates(Polyhedron.Vertices[VertexIndex]);
          double U = SphereProjectedVertex.Y;
          if (U - PinU < -0.5) U += 1.0;
          else if (U - PinU > 0.5) U -= 1.0;
          double V = SphereProjectedVertex.Z;
          if (V - PinV < -0.5) V += 1.0;
          else if (V - PinV > 0.5) V -= 1.0;

          MeshUVs.Add(FVector2D(U, V));
        }
      } else if (UVGeneration == EPolyhedronUVGeneration::Cubic) {
        // Select the Face of the cube based on the Normal.
        EPolyhedronCubicFace Pin = ChooseFaceForCubicProjection(PolygonNormal);
        for (int32 VertexIndex : Polygon.VertexIndices) {
          // Project each coordinate unto the cube.
          FVector2D UV = ProjectOntoCube(Polyhedron.Vertices[VertexIndex].GetSafeNormal(), Pin);
          MeshUVs.Add((UV + FVector2D(1.0, 1.0)) / FVector2D(2.0, 2.0));
        }
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
