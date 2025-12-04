// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAssetManager.h"
#include "AbilitySystemGlobals.h"

UCoreAssetManager& UCoreAssetManager::Get()
{
	check(GEngine);
	UCoreAssetManager* HDAssetManager = Cast<UCoreAssetManager>(GEngine->AssetManager);
	return *HDAssetManager;
}

void UCoreAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp, Warning, TEXT("Hello World"));
}
