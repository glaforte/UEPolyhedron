// Copyright 2024 TabbyCoder

#pragma once

#include "EngineUtils.h"

// Write directly on the screen, in DEBUG mode.
void POLYHEDRON_API WriteOnScreen(const FString& Text);
void POLYHEDRON_API WriteOnScreen(const TCHAR* TextFormat, ...);

// Report Errors as a non-exception-throwing alternative to check().
void POLYHEDRON_API ReportError(const char* FunctionName, const char* TextFormat, ...);
#define REPORT_ERROR(TextFormat, ...) ReportError(__FUNCTION__, TextFormat, ##__VA_ARGS__)

#define REPORT_ERROR_IF(CheckExpression, TextFormat, ...) \
  if ((CheckExpression)) { ReportError(__FUNCTION__, TextFormat, ##__VA_ARGS__); return; }

#define REPORT_ERROR_RETURN_IF(CheckExpression, ReturnValue, TextFormat, ...) \
  if ((CheckExpression)) { ReportError(__FUNCTION__, TextFormat, ##__VA_ARGS__); return (ReturnValue); }

#if WITH_AUTOMATION_TESTS && WITH_EDITORONLY_DATA
// Returns the first actor of a given type and name in the world.
template <class ActorClassT> ActorClassT* FindActorInWorld(UWorld* World, const FName& ActorName) {
  for (TActorIterator<AActor> It(World); It; ++It) {
    if (It->GetFName() == ActorName || It->GetActorLabel() == ActorName) {
      ActorClassT* TypedActor = Cast<ActorClassT>(*It);
      if (TypedActor != nullptr) return TypedActor;
    }
  }
  return nullptr;
}

// Counts the number of actors of the given type in the world.
template <class ActorClassT> int32 CountActorsInWorld(UWorld* World) {
  int32 ActorCount = 0;
  for (TActorIterator<ActorClassT> It(World); It; ++It) { ++ActorCount; }
  return ActorCount;
}

#endif
