
#include "Player/CorePlayerState.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/CorePlayerCharacter.h"
#include "Net/UnrealNetwork.h"

ACorePlayerState::ACorePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("AttributeSetBase"));

	SetNetUpdateFrequency(100.f);
	
}

void ACorePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACorePlayerState, Level);
	DOREPLIFETIME(ACorePlayerState, XP);
	DOREPLIFETIME(ACorePlayerState, AttributePoints);
	DOREPLIFETIME(ACorePlayerState, AbilityPoints);
}

UAbilitySystemComponent* ACorePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACorePlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ACorePlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void ACorePlayerState::AddToAttributePoints(int32 InPoints)
{
	AttributePoints += InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ACorePlayerState::AddToAbilityPoints(int32 InPoints)
{
	AbilityPoints += InPoints;
	OnAbilityPointsChangedDelegate.Broadcast(AbilityPoints);
}

void ACorePlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level, false);
}

void ACorePlayerState::SetToAttributePoints(int32 InPoints)
{
	AttributePoints = InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ACorePlayerState::SetToAbilityPoints(int32 InPoints)
{
	AbilityPoints = InPoints;
	OnAbilityPointsChangedDelegate.Broadcast(AbilityPoints);
}

void ACorePlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ACorePlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void ACorePlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void ACorePlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ACorePlayerState::OnRep_AbilityPoints(int32 OldAbilityPoints)
{
	OnAbilityPointsChangedDelegate.Broadcast(AbilityPoints);
}
