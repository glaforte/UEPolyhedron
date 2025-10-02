// Copyright 2024 TabbyCoder

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "ProceduralMeshComponent.h"
#include "PolyhedronPolygonComponent.generated.h"

/**
 * UPolyhedronPolygonComponent
 */
UCLASS()
class POLYHEDRON_API UPolyhedronPolygonComponent : public UProceduralMeshComponent {
  GENERATED_BODY()

public:
  UPolyhedronPolygonComponent(const FObjectInitializer& ObjectInitializer);

public: // ProceduralMesh Generation
  void Reset(int32 ComponentIndex = -1); // -1 resets all indices
  void SetPolyhedronPolygon(int32 ComponentIndex, const FPolyhedronMesh& PolyhedronMesh, int32 PolygonIndex, float Offset);
  void SetPolyhedronPolygon(int32 ComponentIndex, const FPolyhedronMesh& PolyhedronMesh, const TArray<int32>& PolygonIndices, float Offset);
};
