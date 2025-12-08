
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CorePlayerController.generated.h"

class ACorePlayerCharacter;
class UDamageTextComponent;
class ACoreHUD;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCoreAbilitySystemComponent;

UCLASS()
class GASCORE_API ACorePlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	ACorePlayerController();
	/* Player Input */
	virtual void SetupInputComponent() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	

	UFUNCTION(BlueprintCallable)
	void SprintButtonPressed();
	UFUNCTION(BlueprintCallable)
	void SprintButtonReleased();
	
	/* HUD */
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACorePlayerCharacter> PlayerCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<ACoreHUD> BaseHUD;
	
	/* Player Input */
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputConfig;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	UCoreAbilitySystemComponent* GetASC();
	
	UPROPERTY()
	TObjectPtr<UCoreAbilitySystemComponent> CoreAbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

public:

};
