// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "Player/CoreCharacter.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "CoreEnemy.generated.h"


class UBehaviorTree;
class AEnemyAIController;
class UWidgetComponent;
class ABaseWeapon;

UCLASS()
class GASCORE_API ACoreEnemy : public ACoreCharacter
{
	GENERATED_BODY()

public:
	ACoreEnemy(const class FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

public:

	/* Public Setters and Getters */
	
	void SetLevel(int32 InLevel) { Level = InLevel; }
	/** Combat Interface **/
	virtual int32 GetPlayerLevel_Implementation() override;
	

};
