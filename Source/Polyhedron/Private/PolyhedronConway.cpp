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
}

void APolyhedronConway::BeginPlay() {
  Super::BeginPlay();

  if (PolyhedronComponent == nullptr) {
    CreatePolyhedronComponent();
  }
}

void APolyhedronConway::PostLoad() {
  Super::PostLoad();

  if (PolyhedronComponent == nullptr) {
    CreatePolyhedronComponent();
  }
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
  if (Property != nullptr && Property->HasMetaData("Recreate")) {
    if (PolyhedronComponent == nullptr) {
      CreatePolyhedronComponent();
    } else {
      GeneratePolyhedron();
    }
  }
}
#endif

void APolyhedronConway::CreatePolyhedronComponent() {
  REPORT_ERROR_IF(PolyhedronComponent != nullptr, "PolyhedronComponent already exists");
  PolyhedronComponent = NewObject<UPolyhedronComponent>(this, TEXT("PolyhedronComponent"), RF_Transient);
  PolyhedronComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
  GeneratePolyhedron();
}

void APolyhedronConway::GeneratePolyhedron() {
  REPORT_ERROR_IF(PolyhedronComponent == nullptr, "Missing PolyhedronComponent");
  REPORT_ERROR_IF(ConwayPolyhedronNotation.Len() < 1, "Empty ConwayPolyhedronNotation makes no Polyhedron");

  FPolyhedronTools PolyhedronTools;
  Polyhedron = PolyhedronTools.GenerateFromConwayPolyhedronNotation(ConwayPolyhedronNotation, Scale);
  PolyhedronComponent->SetPolyhedronMesh(Polyhedron, bEnableCollision);
}
