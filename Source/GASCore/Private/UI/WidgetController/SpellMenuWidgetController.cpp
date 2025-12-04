// Copyright DM


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "CoreGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetBasePS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetCoreASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			FString Description;
			FString NextLevelDescription;
			GetCoreASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			FCoreAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetCoreASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetBasePS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChanged.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		FString Description;
		FString NextLevelDescription;
		GetCoreASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	
	const int32 SpellPoints = GetBasePS()->GetSpellPoints();
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GasTag::Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetCoreASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid =  AbilitySpec != nullptr;
	
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GasTag::Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetCoreASC()->GetStatusFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	FString Description;
	FString NextLevelDescription;
	GetCoreASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetCoreASC())
	{
		GetCoreASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.Ability = GasTag::Abilities_None;
	SelectedAbility.Status = GasTag::Abilities_Status_Locked;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;
	
	const FGameplayTag SelectedStatus = GetCoreASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(GasTag::Abilities_Status_Equipped))
	{
		SelectedSlot = GetCoreASC()->GetSlotFromAbilityTag(SelectedAbility.Ability);	
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FName& SlotName, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;
	// Check selected ability against the slots ability type.
	// (Don't equip an offensive spell in a passive slot and vice versa)
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetCoreASC()->ServerEquipAbility(SelectedAbility.Ability, SlotName);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FName& Slot, const FName& PreviousSlot)
{
	bWaitingForEquipSelection = false;
	
	FCoreAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GasTag::Abilities_Status_Unlocked;
	LastSlotInfo.InputName = PreviousSlot;
	LastSlotInfo.AbilityTag = GasTag::Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell.
	AbilityInfoDelegate.Broadcast(LastSlotInfo);
	
	FCoreAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputName =  Slot;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
	
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{

	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;
	
	if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}
