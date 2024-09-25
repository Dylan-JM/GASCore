// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyGameplayTags.h"

namespace GasTag
{

	/* Primary Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength, "Attributes.Primary.Strength", "A Gameplay Tag for Strength.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence", "A Gameplay Tag for Intelligence.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience, "Attributes.Primary.Resilience", "A Gameplay Tag for Resilience.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor, "Attributes.Primary.Vigor", "A Gameplay Tag for Vigor.")

	/* Secondary Attributes */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor, "Attributes.Secondary.Armor", "A Gameplay Tag for armor.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration", "A Gameplay Tag for ArmorPenetration.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance", "A Gameplay Tag for BlockChance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitChance, "Attributes.Secondary.CriticalHitChance", "A Gameplay Tag for CriticalHitChance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitDamage, "Attributes.Secondary.CriticalHitDamage", "A Gameplay Tag for CriticalHitDamage.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance, "Attributes.Secondary.CriticalHitResistance", "A Gameplay Tag for CriticalHitResistance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegeneration, "Attributes.Secondary.HealthRegeneration", "A Gameplay Tag for HealthRegeneration.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegeneration, "Attributes.Secondary.ManaRegeneration", "A Gameplay Tag for ManaRegeneration.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_StaminaRegeneration, "Attributes.Secondary.StaminaRegeneration", "A Gameplay Tag for StaminaRegeneration.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth", "A Gameplay Tag for max health.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana", "A Gameplay Tag for max mana.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxStamina, "Attributes.Secondary.MaxStamina", "A Gameplay Tag for max stamina.")

	
	/* Vital Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health", "A Gameplay Tag for health.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana", "A Gameplay Tag for mana.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Stamina, "Attributes.Vital.Stamina", "A Gameplay Tag for stamina.")

	/* Resistance Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "A Gameplay Tag for fire resistance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "A Gameplay Tag for Lightning resistance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "A Gameplay Tag for Arcane resistance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "A Gameplay Tag for Physical resistance.")


	/* Meta Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Meta_IncomingXP, "Attributes.Meta.IncomingXP", "A Gameplay Tag for Incoming XP.")

	/* Damage Tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "A Gameplay Tag for damage.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire Damage Type.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning Damage Type.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane Damage Type.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical Damage Type.")

	/* Debuff Tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Burn, "Debuff.Burn", "A Gameplay Tag for Burn Debuff.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Stun, "Debuff.Stun", "A Gameplay Tag for Stun Debuff.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Arcane, "Debuff.Arcane", "A Gameplay Tag for Arcane Debuff.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Physical, "Debuff.Physical", "A Gameplay Tag for Physical Debuff.")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Chance, "Debuff.Chance", "A Gameplay Tag for Debuff Chance.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Damage, "Debuff.Damage", "A Gameplay Tag for Debuff Damage.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Duration, "Debuff.Duration", "A Gameplay Tag for Debuff Duration.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Frequency, "Debuff.Frequency", "A Gameplay Tag for Debuff Frequency.")

	/* Mapping damage types to associated debuffs */
	const TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs =
	{
		{Damage_Fire, Debuff_Burn},
		{Damage_Lightning, Debuff_Stun},
		{Damage_Arcane, Debuff_Arcane},
		{Damage_Physical, Debuff_Physical},
	};

	/* Mapping damage types to the associated resistances */
	const TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances =
	{
		{Damage_Fire, Attributes_Resistance_Fire},
		{Damage_Lightning, Attributes_Resistance_Lightning},
		{Damage_Arcane, Attributes_Resistance_Arcane},
		{Damage_Physical, Attributes_Resistance_Arcane},
	};

	/* Input Tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_InputPressed, "Player.Block.InputPressed", "A Gameplay Tag for blocking input pressed.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_InputReleased, "Player.Block.InputReleased", "A Gameplay Tag for blocking input released.")

	/* Effect Tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_HitReact, "Effect.HitReact", "A Gameplay Tag for hit reacting.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_StaminaRegen, "Effect.StaminaRegen", "A Gameplay Tag for stamina regen.")

	/* Ability Tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Equipped, "Abilities.Status.Equipped", "Equipped Status Tag.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Unlocked, "Abilities.Status.Unlocked", "Unlocked Status Tag.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_None, "Abilities.None", "No Ability Tag.")
}
