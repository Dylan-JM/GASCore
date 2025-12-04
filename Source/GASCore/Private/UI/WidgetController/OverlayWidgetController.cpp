// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/CoreGameplayTags.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	
	OnHealthChanged.Broadcast(GetHDAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetHDAttributeSet()->GetMaxHealth());
	
	OnManaChanged.Broadcast(GetHDAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetHDAttributeSet()->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetBasePS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetBasePS()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel, bool bLevelUp)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
	});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnStaminaChanged.Broadcast(Data.NewValue);
	}
);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHDAttributeSet()->GetMaxStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxStaminaChanged.Broadcast(Data.NewValue);
		}
	);

	if (GetHDASC())
	{
		GetHDASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (GetHDASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetHDASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		GetHDASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// For example, say that Tag = Message.HealthPotion
					// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetBasePS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out BasePlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelUpRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) /  static_cast<float>(DeltaLevelUpRequirement);
		
		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	
	FHDAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GasTag::Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GasTag::Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell.
	AbilityInfoDelegate.Broadcast(LastSlotInfo);
	
	FHDAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag =  Slot;
	AbilityInfoDelegate.Broadcast(Info);
	
}
