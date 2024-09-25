// Copyright DM


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "Components/MyAbilitySystemComponent.h"
#include "Interfaces/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UMyAbilitySystemComponent* HDASC = Cast<UMyAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		HDASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		ActivateIfEquipped(HDASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UMyAbilitySystemComponent* HDASC = Cast<UMyAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				HDASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				ActivateIfEquipped(HDASC);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UMyAbilitySystemComponent* HDASC)
{
	const bool bStartupAbilitiesGiven = HDASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (HDASC->GetStatusFromAbilityTag(PassiveSpellTag) == GasTag::Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
