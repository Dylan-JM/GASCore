// Copyright DM


#include "HUD/WidgetController/WidgetController.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "PlayerController/BasePlayerController.h"
#include "PlayerState/BasePlayerState.h"

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
	if (!GetHDASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FHDAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(HDAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.StatusTag = HDAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetHDASC()->ForEachAbility(BroadcastDelegate);
}

ABasePlayerController* UWidgetController::GetBasePC()
{
	if (BasePlayerController == nullptr)
	{
		BasePlayerController = Cast<ABasePlayerController>(PlayerController);
	}
	return BasePlayerController;
}

ABasePlayerState* UWidgetController::GetBasePS()
{
	if (BasePlayerState == nullptr)
	{
		BasePlayerState = Cast<ABasePlayerState>(PlayerState);
	}
	return BasePlayerState;
}

UMyAbilitySystemComponent* UWidgetController::GetHDASC()
{
	if (HDAbilitySystemComponent == nullptr)
	{
		HDAbilitySystemComponent = Cast<UMyAbilitySystemComponent>(AbilitySystemComponent);
	}
	return HDAbilitySystemComponent;
}

UMyAttributeSet* UWidgetController::GetHDAttributeSet()
{
	if (HDAttributeSet == nullptr)
	{
		HDAttributeSet = Cast<UMyAttributeSet>(AttributeSet);
	}
	return HDAttributeSet;
}
