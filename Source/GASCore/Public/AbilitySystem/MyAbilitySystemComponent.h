// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetsTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*Ability Tag */, const FGameplayTag& /*Status Tag*/, int32 /*AbilityLeve*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*Ability Tag */, const FGameplayTag& /*Status */, const FGameplayTag& /*Slot */, const FGameplayTag& /*PrevSlot*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /*AbilityTag*/, bool /*bActivate*/)

class ULoadScreenSaveGame;

UCLASS()
class GASCORE_API UMyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	void AddCharacterAbility(const TSubclassOf<UGameplayAbility>& AbilityToAdd);
	
	bool bStartupAbilitiesGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);
	
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	
	void UpdateAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);



	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;
	
protected:

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
