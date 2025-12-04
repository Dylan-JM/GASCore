// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreGameMode.generated.h"


class UAbilityInfo;

UCLASS(minimalapi)
class ACoreGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ACoreGameMode();

	UPROPERTY(EditDefaultsOnly, Category= "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};



