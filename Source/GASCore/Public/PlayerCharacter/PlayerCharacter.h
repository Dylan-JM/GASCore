// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Interface/PlayerInterface.h"
#include "PlayerCharacter.generated.h"

class AHealthCharacter;
class ABaseEnemy;
class AEquippableActor;
class UNiagaraComponent;
enum class ECharacterAbilityInputID : uint8;
enum class EActionState : uint8;
class ABasePlayerController;
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
class GASCORE_API APlayerCharacter : public ABaseCharacter, public IPlayerInterface
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
	TObjectPtr<ABasePlayerController> PlayerCharacterController;
	
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
	
	/* Combat Interface */
	//virtual void Die(const FVector& DeathImpulse, bool bRagdoll = true) override;
	
	/* End Combat Interface */
	
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;
	
	/* Camera Getters */
	UFUNCTION(BlueprintCallable)
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/* End Camera Getters */
	
	UFUNCTION(BlueprintCallable)
	FHitResult GetTraceResult(float TraceDistance, bool bUseVisibilityChannel = false);
	
	void SetSprinting(bool bInIsSprinting);
	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bInIsSprinting);

	UPROPERTY(Replicated)
	bool bIsSprinting = false;
	
};
