// Copyright DM


#include "UI/WidgetController/AbilityMenuWidgetController.h"
#include "CoreGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerState.h"

void UAbilityMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	AbilityPointsChanged.Broadcast(GetCorePS()->GetAbilityPoints());
}

void UAbilityMenuWidgetController::BindCallbacksToDependencies()
{
	GetCoreASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentAbilityPoints, bEnableSpendPoints, bEnableEquip);
			FString Description;
			FString NextLevelDescription;
			GetCoreASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
			AbilityGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			FCoreAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetCoreASC()->AbilityEquipped.AddUObject(this, &UAbilityMenuWidgetController::OnAbilityEquipped);

	GetCorePS()->OnAbilityPointsChangedDelegate.AddLambda([this](int32 AbilityPoints)
	{
		AbilityPointsChanged.Broadcast(AbilityPoints);
		CurrentAbilityPoints = AbilityPoints;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentAbilityPoints, bEnableSpendPoints, bEnableEquip);
		FString Description;
		FString NextLevelDescription;
		GetCoreASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		AbilityGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	});
}

void UAbilityMenuWidgetController::AbilityGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	
	const int32 AbilityPoints = GetCorePS()->GetAbilityPoints();
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
	ShouldEnableButtons(AbilityStatus, AbilityPoints, bEnableSpendPoints, bEnableEquip);
	FString Description;
	FString NextLevelDescription;
	GetCoreASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	AbilityGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	
}

void UAbilityMenuWidgetController::SpendPointButtonPressed()
{
	if (GetCoreASC())
	{
		GetCoreASC()->ServerSpendAbilityPoint(SelectedAbility.Ability);
	}
}

void UAbilityMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.Ability = GasTag::Abilities_None;
	SelectedAbility.Status = GasTag::Abilities_Status_Locked;

	AbilityGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void UAbilityMenuWidgetController::EquipButtonPressed()
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

void UAbilityMenuWidgetController::AbilityRowGlobePressed(const FName& SlotName, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;
	// Check selected ability against the slots ability type.
	// (Don't equip an offensive spell in a passive slot and vice versa)
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetCoreASC()->ServerEquipAbility(SelectedAbility.Ability, SlotName);
}

void UAbilityMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FName& Slot, const FName& PreviousSlot)
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
	AbilityGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
	
}

void UAbilityMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 AbilityPoints,
                                                     bool& bShouldEnableAbilityPointsButton, bool& bShouldEnableEquipButton)
{

	bShouldEnableAbilityPointsButton = false;
	bShouldEnableEquipButton = false;
	
	if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (AbilityPoints > 0)
		{
			bShouldEnableAbilityPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Eligible))
	{
		if (AbilityPoints > 0)
		{
			bShouldEnableAbilityPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GasTag::Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (AbilityPoints > 0)
		{
			bShouldEnableAbilityPointsButton = true;
		}
	}
}
