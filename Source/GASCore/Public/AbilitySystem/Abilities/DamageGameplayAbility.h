// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"
#include "CoreGameplayAbility.h"
#include "Interface/CombatInterface.h"
#include "DamageGameplayAbility.generated.h"


class ABaseWeapon;
/**
 * 
 */
UCLASS()
class GASCORE_API UDamageGameplayAbility : public UCoreGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Damage")
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Damage")
	float DebuffChance = 20.f; // Potentially make this a scalable float as well as the debuff damage with level

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackForceMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackChance = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	bool IsBlockable = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceDistance = 10000.f;
	
};
