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
    REPORT_ERROR_IF(PolyhedronComponent != nullptr, "BeginPlay called multiple times");
    PolyhedronComponent = NewObject<UPolyhedronComponent>(this, TEXT("PolyhedronComponent"), RF_Transient);
    PolyhedronComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    GeneratePolyhedron();
  }
}

void APolyhedronConway::PostLoad() {
  Super::PostLoad();

  if (PolyhedronComponent == nullptr) {
    REPORT_ERROR_IF(PolyhedronComponent != nullptr, "BeginPlay called multiple times");
    PolyhedronComponent = NewObject<UPolyhedronComponent>(this, TEXT("PolyhedronComponent"), RF_Transient);
    PolyhedronComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    GeneratePolyhedron();
  }
}

#if WITH_EDITOR
void APolyhedronConway::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
  Super::PostEditChangeProperty(PropertyChangedEvent);

  // Regenerate the Polyhedron only if the user changes the local properties.
	FProperty* Property = PropertyChangedEvent.MemberProperty;
  if (Property != nullptr && PolyhedronComponent != nullptr && Property->HasMetaData("Recreate")) {
    GeneratePolyhedron();
  }
}
#endif

void APolyhedronConway::GeneratePolyhedron() {
  REPORT_ERROR_IF(PolyhedronComponent == nullptr, "Missing PolyhedronComponent");
  REPORT_ERROR_IF(ConwayPolyhedronNotation.Len() < 1, "Empty ConwayPolyhedronNotation makes no Polyhedron");

  FPolyhedronTools PolyhedronTools;
  Polyhedron = PolyhedronTools.GenerateFromConwayPolyhedronNotation(ConwayPolyhedronNotation, Scale);
  PolyhedronComponent->SetPolyhedronMesh(Polyhedron, bEnableCollision);
}
