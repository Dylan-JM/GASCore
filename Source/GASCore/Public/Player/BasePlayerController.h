
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class APlayerCharacter;
class UDamageTextComponent;
class ABaseHUD;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UMyAbilitySystemComponent;

UCLASS()
class GASCORE_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	ABasePlayerController();
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
	TObjectPtr<APlayerCharacter> PlayerCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<ABaseHUD> BaseHUD;
	
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

	UMyAbilitySystemComponent* GetASC();
	
	UPROPERTY()
	TObjectPtr<UMyAbilitySystemComponent> HDAbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

public:

};
