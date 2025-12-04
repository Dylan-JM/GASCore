 // Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"
#include "CoreGameplayTags.h"
#include "AbilitySystem/CoreAbilitySystemLibrary.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Player/CoreCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Player/CorePlayerController.h"

 UCoreAttributeSet::UCoreAttributeSet()
{
	/* Primary Attributes */
	TagsToAttributes.Add(GasTag::Attributes_Primary_Strength,GetStrengthAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Primary_Intelligence,GetIntelligenceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Primary_Resilience,GetResilienceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Primary_Vigor,GetVigorAttribute);

	/* Secondary Attributes */
	TagsToAttributes.Add(GasTag::Attributes_Secondary_Armor,GetArmorAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_ArmorPenetration,GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_BlockChance,GetBlockChanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_CriticalHitChance,GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_HealthRegeneration,GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_ManaRegeneration,GetManaRegenerationAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_StaminaRegeneration,GetStaminaRegenerationAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_MaxHealth,GetMaxHealthAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_MaxMana,GetMaxManaAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Secondary_MaxStamina,GetMaxStaminaAttribute);

	/* Resistance Attributes */
	TagsToAttributes.Add(GasTag::Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GasTag::Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UCoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, StaminaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	// Resistance Attributes

	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
	
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Health, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Mana, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Stamina, COND_None, REPNOTIFY_Always);	
	
}

void UCoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void UCoreAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)
{
	
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACoreCharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACoreCharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
	
}

void UCoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// Checks if character is dead. All checks before make sure there is no nullptr before accessing anything
	if (IsValid(Props.TargetCharacter) && Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle health changes.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Handle mana changes.
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Handle stamina changes.
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
}

void UCoreAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bFatal = NewHealth <= 0;
		if (bFatal)
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if (CombatInterface)
			{
				CombatInterface->Die(UCoreAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
		}
		else
		{
			const FVector& KnockbackForce = UCoreAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}

		const bool bBlock = UCoreAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UCoreAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		if (UCoreAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UCoreAttributeSet::Debuff(const FEffectProperties& Props)
{
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag& DamageType = UCoreAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UCoreAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UCoreAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UCoreAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	//Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);

	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	const FGameplayTag DebuffTag = GasTag::DamageTypesToDebuffs[DamageType];
	TagContainer.Added.AddTag(DebuffTag);

	if(DebuffTag.MatchesTagExact(GasTag::Debuff_Stun))
	{
		TagContainer.Added.AddTag(GasTag::Player_Block_InputPressed);
		TagContainer.Added.AddTag(GasTag::Player_Block_InputReleased);
	}
	
	Component.SetAndApplyTargetTagChanges(TagContainer);

	
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UCoreAttributeSet::GetIncomingDamageAttribute();
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FHDGameplayEffectContext* HDContext = static_cast<FHDGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		HDContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
	
}

void UCoreAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
	
}

void UCoreAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (ACorePlayerController* PlayerController = Cast<ACorePlayerController>(Props.SourceCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (ACorePlayerController* PlayerController = Cast<ACorePlayerController>(Props.TargetCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UCoreAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Strength, OldStrength);
}

void UCoreAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Intelligence, OldIntelligence);
}

void UCoreAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Resilience, OldResilience);
}

void UCoreAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Vigor, OldVigor);
}

void UCoreAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Armor, OldArmor);
}

void UCoreAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UCoreAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, BlockChance, OldBlockChance);
}

void UCoreAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UCoreAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UCoreAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UCoreAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UCoreAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UCoreAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& OldStaminaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, StaminaRegeneration, OldStaminaRegeneration);
}

void UCoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Health, OldHealth);
}

void UCoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxHealth, OldMaxHealth);
}

void UCoreAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Mana, OldMana);
}

void UCoreAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxMana, OldMaxMana);
}

void UCoreAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Stamina, OldStamina);
}

void UCoreAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxStamina, OldMaxStamina);
}

void UCoreAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, FireResistance, OldFireResistance);
}

void UCoreAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, LightningResistance, OldLightningResistance);
}

void UCoreAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UCoreAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, PhysicalResistance, OldPhysicalResistance);
}




