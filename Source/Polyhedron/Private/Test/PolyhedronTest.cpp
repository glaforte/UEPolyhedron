// Copyright 2024 TabbyCoder

#include "CQTest.h"
#include "EngineUtils.h"
#include "Helpers.h"
#include "Polyhedron.h"
#include "PolyhedronConway.h"
#include "Components/MapTestSpawner.h"

#if WITH_AUTOMATION_TESTS && WITH_EDITORONLY_DATA

TEST_CLASS(PolyhedronTest, "Polyhedron.Subdivision") {
  TUniquePtr<FMapTestSpawner> Spawner;
  UWorld* World;

  BEFORE_EACH() {
    // Load the test level.
    Spawner = MakeUnique<FMapTestSpawner>(TEXT("/Polyhedron"), TEXT("PolyhedronTest"));
    Spawner->AddWaitUntilLoadedCommand(TestRunner);
    TestCommandBuilder.Until([&] {
      APawn* Pawn = Spawner->FindFirstPlayerPawn();
      return Pawn != nullptr;
    }).Then([&] {
      World = &Spawner->GetWorld();
      ASSERT_THAT(IsNotNull(World));
    });
  }

  TEST_METHOD(IsocahedronSubdivision) {

    // When this test starts, the map is fully loaded.
    // I built this test map to contain a row of polyhedron.
    // The polyhedron have an increasing level of detail / complexity.
    TestCommandBuilder.Do([&] {
      ASSERT_THAT(AreEqual(CountActorsInWorld<APolyhedronConway>(World), 7));

      // The first polyhedron is the created Isocahedron, which has 20 triangles and 12 vertices.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 12));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 20));
    }).Do([&] {
      // The Dodecahedron has 20 vertices and 12 pentagons.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("Dodecahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 20));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 12));
    }).Do([&] {
      // Truncated Icosahedron.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("Truncated Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 60));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 32));
    }).Do([&] {
      // 1st Level of Subdivision on the Truncated Icosahedron.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("TKT Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 540));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 272));
    }).Do([&] {
      // 2nd Level of Subdivision on the Truncated Icosahedron.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("TK^2T Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 4860));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 2432));
    }).Do([&] {
      // 3rd Level of Subdivision on the Truncated Icosahedron.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("TK^3T Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 43740));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 21872));
    }).Do([&] {
      // 4th Level of Subdivision on the Truncated Icosahedron.
      APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, FName("TK^4T Icosahedron"));
      ASSERT_THAT(IsNotNull(Actor));
      const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
      ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), 393660));
      ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), 196832));
    });
  }
};

#endif // WITH_AUTOMATION_TESTS
