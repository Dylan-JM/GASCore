
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Player/BasePlayerState.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "BaseCharacter.generated.h"


class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
class UNiagaraSystem;
class UAnimMontage;
class UGameplayEffect;
class UGameplayAbility;
class UMyAbilitySystemComponent;
class UMyAttributeSet;
class UStartupAbilities;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ABaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityFinished);


UCLASS()
class GASCORE_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	/* GAS */
	void HandleGameplayEffectRemoved(const FActiveGameplayEffectHandle InGEHandle);
	UFUNCTION()
	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& InGERemovalInfo);

	


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse, bool bRagdoll = true);
	
	UPROPERTY(BlueprintAssignable, Category= "Abilities")
	FCharacterDiedDelegate OnCharacterDied;

	
	FOnASCRegistered OnAscRegistered;
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeathDelegate;
	FOnDamageSignature OnDamageDelegate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	
	// Movement 
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bDeadOnGround = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bDead = false;

protected:
	virtual void InitAbilityActorInfo();
	
	/* GAS */
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();
	virtual void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> StaminaRegeneration;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UAttributeSet> AttributeSetBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ActorName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Animations")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	friend UMyAttributeSet;
	
	/* Death */
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	USoundBase* DeathSound;

	UFUNCTION(BlueprintCallable)
	void DeathMontageEnded();

	/* End Death */
	
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float LifeSpan = 5.f;

private:
	
	/* GAS */
	
	UPROPERTY(EditAnywhere, Category="GAS | Abilities")
	TObjectPtr<UStartupAbilities> StartupAbilitiesDataAsset;
	
	UPROPERTY(EditAnywhere, Category="GAS | Abilities")
	TObjectPtr<UStartupAbilities> StartupPassiveAbilitiesDataAsset;
	
	/* End GAS */
	
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSetBase; }
	
	/* Combat Interface */
	virtual void Die(const FVector& DeathImpulse, bool bRagdoll = true) override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
};

