// Copyright 2024 TabbyCoder

#include "CQTest.h"
#include "EngineUtils.h"
#include "Helpers.h"
#include "Polyhedron.h"
#include "PolyhedronConway.h"
#include "Components/MapTestSpawner.h"

#if WITH_AUTOMATION_TESTS && WITH_EDITORONLY_DATA

TEST_CLASS(PolyhedronTest, "Polyhedron") {
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

  TEST_METHOD(Subdivision) {

    // When this test starts, the map is fully loaded.
    // I built this test map with many polyhedra with different operations.
    // The polyhedra have an increasing level of detail / complexity.
    TestCommandBuilder.Do([&] {

      auto CheckPolyhedron = [&] (const FName& ActorName, int32 ExpectedVertexCount, int32 ExpectedPolygonCount) {
        APolyhedronConway* Actor = FindActorInWorld<APolyhedronConway>(World, ActorName);
        ASSERT_THAT(IsNotNull(Actor));
        const FPolyhedronMesh& Polyhedron = Actor->GetPolyhedron();
        ASSERT_THAT(AreEqual(Polyhedron.GetVertexCount(), ExpectedVertexCount));
        ASSERT_THAT(AreEqual(Polyhedron.GetPolygonCount(), ExpectedPolygonCount));
      };

      // The Isocahedron has 20 triangles and 12 vertices.
      CheckPolyhedron(FName("Icosahedron"), 12, 20);

      // The Dodecahedron has 20 vertices and 12 pentagons.
      CheckPolyhedron(FName("Dodecahedron"), 20, 12);
      CheckPolyhedron(FName("Dual Icosahedron"), 20, 12);

      // Truncated Icosahedron.
      CheckPolyhedron(FName("Truncated Icosahedron"), 60, 32);

      // Levels of Subdivision on the Truncated Icosahedron.
      CheckPolyhedron(FName("TKT Icosahedron"), 540, 272);
      CheckPolyhedron(FName("TK^2T Icosahedron"), 4860, 2432);
      CheckPolyhedron(FName("TK^3T Icosahedron"), 43740, 21872);
      CheckPolyhedron(FName("TK^4T Icosahedron"), 393660, 196832);

      // Different operations on the Cube.
      CheckPolyhedron(FName("Cube"), 8, 6);
      CheckPolyhedron(FName("Dual Cube"), 6, 8);
      CheckPolyhedron(FName("Ambo Cube"), 12, 14);
      CheckPolyhedron(FName("Gyro Cube"), 38, 24);
      CheckPolyhedron(FName("Truncated Gyro Cube"), 120, 62);
      CheckPolyhedron(FName("Snub Cube"), 24, 38);
      CheckPolyhedron(FName("Ortho Cube"), 26, 24);
      CheckPolyhedron(FName("Expand Cube"), 24, 26);
      CheckPolyhedron(FName("Meta Cube"), 26, 48);
      CheckPolyhedron(FName("Bevel Cube"), 48, 26);
      CheckPolyhedron(FName("Chamfer Cube"), 32, 18);
      CheckPolyhedron(FName("Needle Cube"), 14, 24);
    });
  }
};



TEST_CLASS(PolyhedronPerformanceTest, "Polyhedron") {
  TUniquePtr<FMapTestSpawner> Spawner;
  UWorld* World;

  BEFORE_EACH() {
    // Load the test level.
    Spawner = MakeUnique<FMapTestSpawner>(TEXT("/Polyhedron"), TEXT("PolyhedronPerformanceTest"));
    Spawner->AddWaitUntilLoadedCommand(TestRunner);
    TestCommandBuilder.Until([&] {
      APawn* Pawn = Spawner->FindFirstPlayerPawn();
      return Pawn != nullptr;
    }).Then([&] {
      World = &Spawner->GetWorld();
      ASSERT_THAT(IsNotNull(World));
    });
  }

  TEST_METHOD(Performance) {

    // Just load the level and report on the performance.
    TestCommandBuilder.Do([&] {
    });
  }
};


#endif // WITH_AUTOMATION_TESTS
