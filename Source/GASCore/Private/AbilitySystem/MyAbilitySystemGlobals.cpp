// Copyright DM


#include "AbilitySystem/MyAbilitySystemGlobals.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"

FGameplayEffectContext* UMyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FHDGameplayEffectContext();
}
