// Fill out your copyright notice in the Description page of Project Settings.

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
