// Copyright DM

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MyGameplayAbility.h"
#include "PassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASCORE_API UPassiveAbility : public UMyGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
