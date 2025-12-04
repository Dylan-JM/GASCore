// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"

/* GameplayTags
 *
 * Singleton containing native Gameplay Tags
 * 
*/


namespace GasTag
{

	/* Primary Attributes */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor);


	/* Secondary Attributes */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitChance);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitDamage);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitResistance);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegeneration);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegeneration);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_StaminaRegeneration);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxStamina);
	
	/* Vital Attributes */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Health);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Stamina);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Mana);

	/* Resistance Attributes */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Fire);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Lightning);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Arcane);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Physical);
	
	/* Meta Attributes */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Meta_IncomingXP);

	/* Damage Tags */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Fire);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Lightning);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Arcane);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical);

	/* Debuff Tags */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Burn);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Stun);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Arcane);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Physical);
	
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Chance);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Damage);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Duration);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Frequency);


	/* Input Tags */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Block_InputPressed);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Block_InputReleased;);

	/* Effect Tags */
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_HitReact);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_StaminaRegen);

	/* TMaps */
	GASCORE_API extern const TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	GASCORE_API extern const TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	
	/* Ability Tags */
	
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Status_Equipped);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Status_Unlocked);
	
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_None);
}
