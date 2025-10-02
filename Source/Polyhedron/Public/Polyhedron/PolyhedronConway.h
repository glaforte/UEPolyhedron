// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/

#pragma once

#include "CoreMinimal.h"
#include "Polyhedron.h"
#include "PolyhedronComponent.h"
#include "PolyhedronConway.generated.h"

class APolyhedronConway;

/**
 * This Actor displays a polyhedron determined by a Conway Polyhedron Notation string.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class POLYHEDRON_API APolyhedronConway : public AActor {
	GENERATED_BODY()

public:
	APolyhedronConway();

public: // Event-Handlers
	void BeginPlay() override;
	void PostLoad() override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public: // Polyhedron Definition
	const FPolyhedronMesh& GetPolyhedron() const { return Polyhedron; }
protected: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Polyhedron", meta = (Recreate)) FString ConwayPolyhedronNotation = TEXT("I");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Polyhedron", meta = (Recreate)) float Scale = 100.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Polyhedron", meta = (Recreate)) bool bEnableCollision = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Polyhedron", meta = (AttachMaterial)) TObjectPtr<UMaterialInterface> Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Polyhedron", meta = (Recreate)) EPolyhedronUVGeneration UVGeneration = EPolyhedronUVGeneration::Spherical;
private:
	FPolyhedronMesh Polyhedron;

protected: // Polyhedron Component
	void GeneratePolyhedron();
	void AttachMaterial();
protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Polyhedron") TObjectPtr<UPolyhedronComponent> PolyhedronComponent;
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Polyhedron") int32 VertexCount = 0;
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Polyhedron") int32 PolygonCount = 0;
};
