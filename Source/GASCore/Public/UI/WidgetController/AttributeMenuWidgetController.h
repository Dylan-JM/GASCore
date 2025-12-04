// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;
struct FCoreAttributeInfo;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FHDAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GASCORE_API UAttributeMenuWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category= "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category= "GAS|Attributes")
	FOnPlayerStateChangedSignature AttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable)
	void UpdateAttribute(const FGameplayTag& AttributeTag);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadCastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const; 
};
