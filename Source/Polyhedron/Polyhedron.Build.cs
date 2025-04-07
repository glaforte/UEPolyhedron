// Copyright 2024 TabbyCoder

using UnrealBuildTool;

public class Polyhedron : ModuleRules
{
	public Polyhedron(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Standard Libraries
		PublicDependencyModuleNames.AddRange(new string[] { "Core" });
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine" });

		// Procedural Mesh is the main UE interface for the Polyhedron.
		PublicDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });

		// Automated Testing
		PrivateDependencyModuleNames.AddRange(new string[] { "AutomationTest", "CQTest" });

		// Available options that aren't used yet...
		PublicIncludePaths.AddRange(new string[] {} );
		PrivateIncludePaths.AddRange(new string[] {} );
		DynamicallyLoadedModuleNames.AddRange(new string[] {});
	}
}
