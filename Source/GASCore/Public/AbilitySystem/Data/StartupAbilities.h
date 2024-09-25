// Copyright DM.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StartupAbilities.generated.h"

class UGameplayAbility;
/**
 * 
 */
UCLASS()
class GASCORE_API UStartupAbilities : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
