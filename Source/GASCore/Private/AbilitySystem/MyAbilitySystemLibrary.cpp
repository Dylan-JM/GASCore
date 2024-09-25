// Copyright DM


#include "AbilitySystem/MyAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "GASCoreGameMode.h"
#include "HUD/BaseHUD.h"
#include "HUD/WidgetController/WidgetController.h"
#include "Interfaces/CombatInterface.h"
#include "PlayerState/BasePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "PlayerCharacter/BaseCharacter.h"

bool UMyAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, ABaseHUD*& OutBaseHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutBaseHUD = Cast<ABaseHUD>(PC->GetHUD());
		if (OutBaseHUD)
		{
			ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>();
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

UOverlayWidgetController* UMyAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABaseHUD* BaseHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BaseHUD))
	{
		return BaseHUD->GetOverlayWidgetController(WCParams);
	}
	
	return nullptr;
}

void UMyAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, float Level,
	UAbilitySystemComponent* ASC)
{
	ABaseCharacter* AvatarActor = Cast<ABaseCharacter>(ASC->GetAvatarActor());
	
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

void UMyAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UMyAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
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

void UMyAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
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

void UMyAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

bool UMyAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->IsBlockedHit();
	}
	return false;
}

bool UMyAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->IsCriticalHit();
	}
	return false;
}

bool UMyAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UMyAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UMyAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UMyAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UMyAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (HDEffectContext->GetDamageType().IsValid())
		{
			return *HDEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UMyAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UMyAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UMyAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->IsRadialDamage();
	}
	return false;
}

float UMyAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UMyAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UMyAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

bool UMyAbilitySystemLibrary::IsBlockable(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHDGameplayEffectContext* HDEffectContext = static_cast<const FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HDEffectContext->IsBlockable();
	}
	return false;
}

UCurveTable* UMyAbilitySystemLibrary::GetDamageCalculationCoefficients(AActor* TargetActor)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(TargetActor);
	return BaseCharacter->DamageCalculationCoefficients;
}

void UMyAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UMyAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UMyAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UMyAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float bInDamage)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetDebuffDamage(bInDamage);
	}
}

void UMyAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float bInDuration)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetDebuffDuration(bInDuration);
	}
}

void UMyAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float bInFrequency)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetDebuffFrequency(bInFrequency);
	}
}

void UMyAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		HDEffectContext->SetDamageType(DamageType);
	}
}

void UMyAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UMyAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InForce)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetKnockbackForce(InForce);
	}
}

void UMyAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsRadialDamage)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UMyAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InInnerRadius)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UMyAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InOuterRadius)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UMyAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InOrigin)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UMyAbilitySystemLibrary::SetIsBlockable(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsBlockable)
{
	if (FHDGameplayEffectContext* HDEffectContext = static_cast<FHDGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HDEffectContext->SetIsBlockable(bInIsBlockable);
	}
}

void UMyAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
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

void UMyAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
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

bool UMyAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UMyAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
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

TArray<FRotator> UMyAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
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

TArray<FVector> UMyAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
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

TArray<FVector> UMyAbilitySystemLibrary::EvenlySpacedVectors(const FVector& Start, const FVector& End, float Spread,
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


