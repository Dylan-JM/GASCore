// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CoreGameplayAbility.h"
#include "PassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASCORE_API UPassiveAbility : public UCoreGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
