// Copyright 2024 TabbyCoder
// Based on the Polyhedronisme project.  Released under the MIT License.  Copyright 2019, Anselm Levskaya.  https://levskaya.github.io/polyhedronisme/ | https://github.com/levskaya/polyhedronisme
// Based on earlier work from George W. Hart.  http://www.georgehart.com/


#include "PolyhedronConway.h"
#include "Helpers.h"
#include "Polyhedron.h"
#include "PolyhedronComponent.h"

APolyhedronConway::APolyhedronConway()
  : AActor() {

  // Create the PolyhedronComponent
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PlacementComponent"));
  PolyhedronComponent = CreateDefaultSubobject<UPolyhedronComponent>(TEXT("PolyhedronComponent"), true);
  PolyhedronComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void APolyhedronConway::BeginPlay() {
  Super::BeginPlay();
}

void APolyhedronConway::PostLoad() {
  Super::PostLoad();

  GeneratePolyhedron();
  AttachMaterial();
}

#if WITH_EDITOR
void APolyhedronConway::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
  Super::PostEditChangeProperty(PropertyChangedEvent);

  // Skip any calls before post-load.
  if (HasAnyFlags(RF_NeedPostLoad)) {
    return;
  }

  // Regenerate the Polyhedron only if the user changes the local properties.
	FProperty* Property = PropertyChangedEvent.MemberProperty;
  if (Property == nullptr) {
    return;
  }
  if (Property->HasMetaData("Recreate")) {
    GeneratePolyhedron();
  } else if (Property->HasMetaData("AttachMaterial")) {
    AttachMaterial();
  }
}
#endif

void APolyhedronConway::GeneratePolyhedron() {
  REPORT_ERROR_IF(PolyhedronComponent == nullptr, "Missing PolyhedronComponent");
  REPORT_ERROR_IF(ConwayPolyhedronNotation.Len() < 1, "Empty ConwayPolyhedronNotation makes no Polyhedron");

  FPolyhedronTools PolyhedronTools;
  Polyhedron = PolyhedronTools.GenerateFromConwayPolyhedronNotation(ConwayPolyhedronNotation, Scale);
  PolyhedronComponent->SetPolyhedronMesh(Polyhedron, bEnableCollision, UVGeneration);

  // Record the statistics values exposed to Blueprint and the user.
  VertexCount = Polyhedron.Vertices.Num();
  PolygonCount = Polyhedron.Polygons.Num();
}

void APolyhedronConway::AttachMaterial() {
  REPORT_ERROR_IF(PolyhedronComponent == nullptr, "Missing PolyhedronComponent");
  PolyhedronComponent->SetMaterial(0, Material);
}
