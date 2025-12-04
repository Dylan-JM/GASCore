// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CoreGameplayTags.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Interface/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UCoreAbilitySystemComponent* CoreASC = Cast<UCoreAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		CoreASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		ActivateIfEquipped(CoreASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UCoreAbilitySystemComponent* CoreASC = Cast<UCoreAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				CoreASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				ActivateIfEquipped(CoreASC);
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

void UPassiveNiagaraComponent::ActivateIfEquipped(UCoreAbilitySystemComponent* CoreASC)
{
	const bool bStartupAbilitiesGiven = CoreASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (CoreASC->GetStatusFromAbilityTag(PassiveSpellTag) == GasTag::Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
