// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

using UnrealBuildTool;
using System.Collections.Generic;

public class GASCoreEditorTarget : TargetRules
{
	public GASCoreEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GASCore");
	}
}
