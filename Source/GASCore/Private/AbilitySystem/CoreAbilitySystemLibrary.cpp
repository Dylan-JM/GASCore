// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "AbilitySystem/CoreAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CoreGameMode.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"
#include "CoreGameplayTags.h"
#include "Interface/CombatInterface.h"
#include "Player/CorePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "Player/CoreCharacter.h"
#include "UI/CoreHUD.h"
#include "UI/WidgetController/WidgetController.h"

bool UCoreAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, ACoreHUD*& OutBaseHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutBaseHUD = Cast<ACoreHUD>(PC->GetHUD());
		if (OutBaseHUD)
		{
			ACorePlayerState* PS = PC->GetPlayerState<ACorePlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSetBase();
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UCoreAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ACoreHUD* BaseHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BaseHUD))
	{
		return BaseHUD->GetOverlayWidgetController(WCParams);
	}
	
	return nullptr;
}

UAttributeMenuWidgetController* UCoreAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ACoreHUD* CoreHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, CoreHUD))
	{
		//return CoreHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

UAbilityMenuWidgetController* UCoreAbilitySystemLibrary::GetAbilityMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ACoreHUD* CoreHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, CoreHUD))
	{
		return CoreHUD->GetAbilityMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UCoreAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, float Level,
                                                            UAbilitySystemComponent* ASC)
{
	ACoreCharacter* AvatarActor = Cast<ACoreCharacter>(ASC->GetAvatarActor());
	
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(AvatarActor->DefaultPrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(AvatarActor->DefaultSecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(AvatarActor->DefaultVitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

UAbilityInfo* UCoreAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const ACoreGameMode* CoreGameMode = Cast<ACoreGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (CoreGameMode == nullptr) return nullptr;

	return CoreGameMode->AbilityInfo;
}

void UCoreAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UCoreAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
	
}

void UCoreAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
	
}

void UCoreAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

bool UCoreAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->IsBlockedHit();
	}
	return false;
}

bool UCoreAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->IsCriticalHit();
	}
	return false;
}

bool UCoreAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UCoreAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UCoreAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UCoreAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UCoreAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (CoreEffectContext->GetDamageType().IsValid())
		{
			return *CoreEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UCoreAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UCoreAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UCoreAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->IsRadialDamage();
	}
	return false;
}

float UCoreAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UCoreAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UCoreAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

bool UCoreAbilitySystemLibrary::IsBlockable(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FCoreGameplayEffectContext* CoreEffectContext = static_cast<const FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return CoreEffectContext->IsBlockable();
	}
	return false;
}

UCurveTable* UCoreAbilitySystemLibrary::GetDamageCalculationCoefficients(AActor* TargetActor)
{
	ACoreCharacter* BaseCharacter = Cast<ACoreCharacter>(TargetActor);
	return BaseCharacter->DamageCalculationCoefficients;
}

void UCoreAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UCoreAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UCoreAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UCoreAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float bInDamage)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetDebuffDamage(bInDamage);
	}
}

void UCoreAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float bInDuration)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetDebuffDuration(bInDuration);
	}
}

void UCoreAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float bInFrequency)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetDebuffFrequency(bInFrequency);
	}
}

void UCoreAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		CoreEffectContext->SetDamageType(DamageType);
	}
}

void UCoreAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UCoreAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InForce)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetKnockbackForce(InForce);
	}
}

void UCoreAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsRadialDamage)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UCoreAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InInnerRadius)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UCoreAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InOuterRadius)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UCoreAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InOrigin)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UCoreAbilitySystemLibrary::SetIsBlockable(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsBlockable)
{
	if (FCoreGameplayEffectContext* CoreEffectContext = static_cast<FCoreGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		CoreEffectContext->SetIsBlockable(bInIsBlockable);
	}
}

void UCoreAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                         TArray<AActor*>& OutOverlappingActors, const TArray<AActor*> ActorsToIgnore, float Radius,
                                                         const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	TArray<FOverlapResult> Overlaps;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UCoreAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0) break;
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UCoreAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UCoreAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);

	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	SetIsBlockable(EffectContextHandle, DamageEffectParams.bBlockable);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GasTag::Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GasTag::Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GasTag::Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GasTag::Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
	
}

TArray<FRotator> UCoreAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, FVector::UpVector);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UCoreAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, FVector::UpVector);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}

TArray<FVector> UCoreAbilitySystemLibrary::EvenlySpacedVectors(const FVector& Start, const FVector& End, float Spread,
	int32 NumVectors)
{
	TArray<FVector> Vectors;
	Vectors.Reserve(NumVectors);

	FVector Diff = (End - Start) / (float) NumVectors;
	for (int i = 0; i < NumVectors; ++i)
	{
		Vectors.Add(Start + (i * Diff));
	}

	return Vectors;
}


