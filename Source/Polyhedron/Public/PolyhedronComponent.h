// Copyright 2024 TabbyCoder

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "ProceduralMeshComponent.h"
#include "PolyhedronComponent.generated.h"

struct FPolyhedronMesh;

/**
 * UPolyhedronComponent
 */
UCLASS()
class POLYHEDRON_API UPolyhedronComponent : public UProceduralMeshComponent {
  GENERATED_BODY()

public:
  UPolyhedronComponent(const FObjectInitializer& ObjectInitializer);

public: // ProceduralMesh Generation
  void SetPolyhedronMesh(const FPolyhedronMesh& PolyhedronMesh, bool bEnableCollision = false, EPolyhedronUVGeneration UVGeneration = EPolyhedronUVGeneration::Spherical);
};
