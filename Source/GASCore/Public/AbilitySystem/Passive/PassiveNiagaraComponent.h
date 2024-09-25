// Copyright DM

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

class UMyAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GASCORE_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UPassiveNiagaraComponent();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:

	virtual void BeginPlay() override;
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
	void ActivateIfEquipped(UMyAbilitySystemComponent* HDASC);
	
};