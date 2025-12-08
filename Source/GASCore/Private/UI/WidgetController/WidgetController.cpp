// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "UI/WidgetController/WidgetController.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerController.h"
#include "Player/CorePlayerState.h"

void UWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UWidgetController::BroadcastInitialValues()
{
	
}

void UWidgetController::BindCallbacksToDependencies()
{
}

void UWidgetController::BroadcastAbilityInfo()
{
	if (!GetCoreASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FCoreAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(CoreAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.StatusTag = CoreAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetCoreASC()->ForEachAbility(BroadcastDelegate);
}

ACorePlayerController* UWidgetController::GetBasePC()
{
	if (BasePlayerController == nullptr)
	{
		BasePlayerController = Cast<ACorePlayerController>(PlayerController);
	}
	return BasePlayerController;
}

ACorePlayerState* UWidgetController::GetBasePS()
{
	if (BasePlayerState == nullptr)
	{
		BasePlayerState = Cast<ACorePlayerState>(PlayerState);
	}
	return BasePlayerState;
}

UCoreAbilitySystemComponent* UWidgetController::GetCoreASC()
{
	if (CoreAbilitySystemComponent == nullptr)
	{
		CoreAbilitySystemComponent = Cast<UCoreAbilitySystemComponent>(AbilitySystemComponent);
	}
	return CoreAbilitySystemComponent;
}

UCoreAttributeSet* UWidgetController::GetCoreAttributeSet()
{
	if (CoreAttributeSet == nullptr)
	{
		CoreAttributeSet = Cast<UCoreAttributeSet>(AttributeSet);
	}
	return CoreAttributeSet;
}
