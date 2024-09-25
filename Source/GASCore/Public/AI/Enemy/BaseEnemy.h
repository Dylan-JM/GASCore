
#pragma once

#include "CoreMinimal.h"
#include "Player/BaseCharacter.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "BaseEnemy.generated.h"


class UBehaviorTree;
class AEnemyAIController;
class UWidgetComponent;
class ABaseWeapon;

UCLASS()
class GASCORE_API ABaseEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy(const class FObjectInitializer& ObjectInitializer);
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
