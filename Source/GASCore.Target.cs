// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

using UnrealBuildTool;
using System.Collections.Generic;

public class GASCoreTarget : TargetRules
{
	public GASCoreTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GASCore");
	}
}
