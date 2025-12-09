// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "GameplayTagContainer.h"
#include "CoreGameplayTags.h"
#include "AbilityMenuWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAbilityGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GASCORE_API UAbilityMenuWidgetController : public UWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature AbilityPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FAbilityGlobeSelectedSignature AbilityGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FAbilityGlobeReassignedSignature AbilityGlobeReassignedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void AbilityGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void AbilityRowGlobePressed(const FName& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FName& Slot, const FName& PreviousSlot);
	

private:
	
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 AbilityPoints, bool& bShouldEnableAbilityPointsButton, bool& bShouldEnableEquipButton);
	FSelectedAbility SelectedAbility = { GasTag::Abilities_None, GasTag::Abilities_Status_Locked };
	int32 CurrentAbilityPoints = 0;
	bool bWaitingForEquipSelection = false;
	FName SelectedSlot;
};
