 // Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "MyAbilityTypes.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "AbilitySystem/MyAbilitySystemLibrary.h"
#include "Components/MyAbilitySystemComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "PlayerCharacter/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BasePlayerController.h"

UMyAttributeSet::UMyAttributeSet()
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

void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, StaminaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	// Resistance Attributes

	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
	
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Mana, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Stamina, COND_None, REPNOTIFY_Always);	
	
}

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
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

void UMyAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)
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
			Props.SourceCharacter = Cast<ABaseCharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ABaseCharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
	
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

void UMyAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
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
				CombatInterface->Die(UMyAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
		}
		else
		{
			const FVector& KnockbackForce = UMyAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}

		const bool bBlock = UMyAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UMyAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		if (UMyAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UMyAttributeSet::Debuff(const FEffectProperties& Props)
{
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag& DamageType = UMyAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UMyAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UMyAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UMyAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	
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
	ModifierInfo.Attribute = UMyAttributeSet::GetIncomingDamageAttribute();
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FHDGameplayEffectContext* HDContext = static_cast<FHDGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		HDContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
	
}

void UMyAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
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

void UMyAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (ABasePlayerController* PlayerController = Cast<ABasePlayerController>(Props.SourceCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (ABasePlayerController* PlayerController = Cast<ABasePlayerController>(Props.TargetCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UMyAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Strength, OldStrength);
}

void UMyAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Intelligence, OldIntelligence);
}

void UMyAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Resilience, OldResilience);
}

void UMyAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Vigor, OldVigor);
}

void UMyAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Armor, OldArmor);
}

void UMyAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UMyAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, BlockChance, OldBlockChance);
}

void UMyAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UMyAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UMyAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UMyAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UMyAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UMyAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& OldStaminaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, StaminaRegeneration, OldStaminaRegeneration);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Mana, OldMana);
}

void UMyAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxMana, OldMaxMana);
}

void UMyAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Stamina, OldStamina);
}

void UMyAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxStamina, OldMaxStamina);
}

void UMyAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, FireResistance, OldFireResistance);
}

void UMyAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, LightningResistance, OldLightningResistance);
}

void UMyAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UMyAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, PhysicalResistance, OldPhysicalResistance);
}




