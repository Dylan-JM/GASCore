// Copyright DM


#include "AbilitySystem/CoreAbilitySystemGlobals.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"

FGameplayEffectContext* UCoreAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FHDGameplayEffectContext();
}
