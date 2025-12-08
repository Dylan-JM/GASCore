// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreCharacter.h"
#include "Interface/PlayerInterface.h"
#include "PlayerCharacter.generated.h"

class AHealthCharacter;
class ACoreEnemy;
class AEquippableActor;
class UNiagaraComponent;
enum class ECharacterAbilityInputID : uint8;
enum class EActionState : uint8;
class ACorePlayerController;
class UTargetSystemComponent;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;
class IEnemyInterface;
class USaveGame;
class ULoadScreenSaveGame;
class UAbilityInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRevived);

UCLASS()
class GASCORE_API APlayerCharacter : public ACoreCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	virtual void Jump() override;
	
	UFUNCTION(BlueprintCallable)
	void TraceUnderCrosshairs(FHitResult& TraceHitResult, float TraceDistance, bool bUseVisibilityChannel = false);
	
	/* Movement mechanics*/
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector2D MovementDirectionVector;
	
	/* End Movement Mechanics */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACorePlayerController> PlayerCharacterController;
	
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly)
	float DefaultTraceDistance = 100.f;
private:

	/* Camera */
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
	
	/**/


public:
	/* Camera Getters */
	UFUNCTION(BlueprintCallable)
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/* End Camera Getters */
	
	UFUNCTION(BlueprintCallable)
	FHitResult GetTraceResult(float TraceDistance, bool bUseVisibilityChannel = false);
	
	/* Sprinting GA */
	void SetSprinting(bool bInIsSprinting);
	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bInIsSprinting);

	UPROPERTY(Replicated)
	bool bIsSprinting = false;
	
	/* */
	
	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributesPointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	/* End Player Interface */
	
	/* Combat Interface */
	//virtual void Die(const FVector& DeathImpulse, bool bRagdoll = true) override;
	
	/* End Combat Interface */
};
