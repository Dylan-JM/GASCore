// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CoreAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASCORE_API UCoreAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UCoreAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
	
};
