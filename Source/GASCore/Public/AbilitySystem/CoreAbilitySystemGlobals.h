// Copyright DM

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "CoreAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GASCORE_API UCoreAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
