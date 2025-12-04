// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


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
