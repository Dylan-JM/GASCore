// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CoreGameplayTags.h"
#include "InputAction.h"
#include "PlayerMappableKeySettings.h"
#include "AbilitySystem/CoreAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/CoreGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interface/PlayerInterface.h"

void UCoreAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UCoreAbilitySystemComponent::ClientEffectApplied);
}

void UCoreAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		if (const UCoreGameplayAbility* GameplayAbility = Cast<UCoreGameplayAbility>(AbilitySpec.Ability))
		{
			//AbilitySpec.DynamicAbilityTags.AddTag(FHDGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UCoreAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(GasTag::Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UCoreAbilitySystemComponent::AddCharacterAbility(const TSubclassOf<UGameplayAbility>& AbilityToAdd)
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityToAdd, 1);
	if (const UCoreGameplayAbility* HDAbility = Cast<UCoreGameplayAbility>(AbilitySpec.Ability))
	{
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(GasTag::Abilities_Status_Equipped);
		GiveAbility(AbilitySpec);
	}
}

void UCoreAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UCoreAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UCoreAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UCoreAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FName UCoreAbilitySystemComponent::GetInputFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	UCoreGameplayAbility* CurrentAbility = Cast<UCoreGameplayAbility>(AbilitySpec.Ability);
	if (!CurrentAbility || !CurrentAbility->ActivationInputAction || !CurrentAbility->ActivationInputAction->GetPlayerMappableKeySettings())
	{
		return FName();
	}

	if (CurrentAbility->ActivationInputAction->GetPlayerMappableKeySettings()->GetMappingName().IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, CurrentAbility->ActivationInputAction->GetPlayerMappableKeySettings()->GetMappingName().ToString());
		return CurrentAbility->ActivationInputAction->GetPlayerMappableKeySettings()->GetMappingName();
	}
	return FName();
}

FName UCoreAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputFromSpec(*Spec);
	}
	return FName();
}

bool UCoreAbilitySystemComponent::SlotIsEmpty(const FName& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

bool UCoreAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FName& Slot)
{
	UCoreGameplayAbility* Ability = Cast<UCoreGameplayAbility>(Spec.Ability.Get());
	return Ability->ActivationInputAction->GetPlayerMappableKeySettings()->GetMappingName() == Slot;
}

bool UCoreAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	UCoreGameplayAbility* Ability = Cast<UCoreGameplayAbility>(Spec.Ability.Get());
	return Ability->ActivationInputAction->GetPlayerMappableKeySettings()->GetMappingName().IsValid();
}

FGameplayAbilitySpec* UCoreAbilitySystemComponent::GetSpecWithSlot(const FName& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		UCoreGameplayAbility* Ability = Cast<UCoreGameplayAbility>(AbilitySpec.Ability.Get());
		if (Ability->ActivationInputAction->GetName() == Slot)
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UCoreAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	const UAbilityInfo* AbilityInfo = UCoreAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FCoreAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(GasTag::Abilities_Type_Passive);
}

void UCoreAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FName& Slot)
{
	ClearSlot(&Spec);
}

void UCoreAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FName Slot = GetInputFromSpec(*Spec);
	// TODO: This
	//Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
}

void UCoreAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
                                                                                bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

FGameplayAbilitySpec* UCoreAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UCoreAbilitySystemComponent::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UCoreAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GasTag::Abilities_Status_Eligible))
		{
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GasTag::Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GasTag::Abilities_Status_Unlocked);
			Status = GasTag::Abilities_Status_Unlocked;
			
		}
		else if (Status.MatchesTagExact(GasTag::Abilities_Status_Equipped) || Status.MatchesTagExact(GasTag::Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UCoreAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FName& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FName& PrevSlot = GetInputFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GasTag::Abilities_Status_Equipped || Status == GasTag::Abilities_Status_Unlocked;
		if (bStatusValid)
		{

			// Handle activation/deactivation for passive abilites

			if (!SlotIsEmpty(Slot)) // There is an ability in this slot already. Deactivate and clear its slot.
			{
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					// is that ability the same as this ability if so, return early.
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GasTag::Abilities_Status_Equipped, Slot, PrevSlot);
						return;
					}

					if (IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					ClearSlot(SpecWithSlot);
				}
			}

			if (!AbilityHasAnySlot(*AbilitySpec)) // Ability doesn't yet have a slot (it's not active)
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GasTag::Abilities_Status_Equipped);
			}
			
			AssignSlotToAbility(*AbilitySpec, Slot);
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, GasTag::Abilities_Status_Equipped, Slot, PrevSlot);
	}
}

void UCoreAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FName& Slot, const FName& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UCoreAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                              FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UCoreGameplayAbility* CoreAbility = Cast<UCoreGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = CoreAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = CoreAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UCoreAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTag(GasTag::Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UCoreGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UCoreAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UCoreAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UCoreAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UCoreAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                                   FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);

}
