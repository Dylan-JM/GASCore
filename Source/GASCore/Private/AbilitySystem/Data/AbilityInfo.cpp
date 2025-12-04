// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "AbilitySystem/Data/AbilityInfo.h"

FHDAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	if (!AbilityTag.IsValid()) return FHDAbilityInfo();
	for (const FHDAbilityInfo Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FHDAbilityInfo();
}
