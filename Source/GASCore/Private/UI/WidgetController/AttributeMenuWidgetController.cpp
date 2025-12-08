// Copyright DM


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);
	for (auto& Pair: GetCoreAttributeSet()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
	[this, Pair](const FOnAttributeChangeData& Data)
	{
		BroadCastAttributeInfo(Pair.Key, Pair.Value());
	});
	}
	
	GetBasePS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		});
	
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UCoreAttributeSet* AS = Cast<UCoreAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair: AS->TagsToAttributes)
	{
		BroadCastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	AttributePointsChangedDelegate.Broadcast(GetBasePS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	UCoreAbilitySystemComponent* CoreASC = CastChecked<UCoreAbilitySystemComponent>(AbilitySystemComponent);
	CoreASC->UpdateAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadCastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
	FCoreAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
