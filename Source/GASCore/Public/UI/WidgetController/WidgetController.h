// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FHDAbilityInfo&, Info);

class UCoreAttributeSet;
class UCoreAbilitySystemComponent;
class ACorePlayerState;
class ACorePlayerController;
class UAttributeSet;
class UAbilitySystemComponent;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY();

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS) :
	PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};


UCLASS()
class GASCORE_API UWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	void BroadcastAbilityInfo();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<ACorePlayerController> BasePlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<ACorePlayerState> BasePlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UCoreAbilitySystemComponent> HDAbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UCoreAttributeSet> HDAttributeSet;

	ACorePlayerController* GetBasePC();
	ACorePlayerState* GetBasePS();
	UCoreAbilitySystemComponent* GetHDASC();
	UCoreAttributeSet* GetHDAttributeSet();
};
