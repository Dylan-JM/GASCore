
#include "PlayerCharacter/BaseCharacter.h"
#include "Engine.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "AbilitySystem/Data/StartupAbilities.h"
#include "AbilitySystem/Debuffs/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Components/MyAbilitySystemComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCore/GASCore.h"
#include "Net/UnrealNetwork.h"


ABaseCharacter::ABaseCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	bAlwaysRelevant = true;
	
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());

	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, bDeadOnGround);
	DOREPLIFETIME(ABaseCharacter, bDead);
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

void ABaseCharacter::AddCharacterAbilities()
{
	UMyAbilitySystemComponent* MASC = Cast<UMyAbilitySystemComponent>(AbilitySystemComponent);
	if (!IsValid(MASC))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MASC is null");
		return;
	}
	// Grant abilities, but only on the server
	if (!HasAuthority()) return;

	if (StartupAbilitiesDataAsset) MASC->AddCharacterAbilities(StartupAbilitiesDataAsset->StartupAbilities);
	if (StartupPassiveAbilitiesDataAsset) MASC->AddCharacterPassiveAbilities(StartupPassiveAbilitiesDataAsset->StartupAbilities);
}

void ABaseCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	// this line makes it so the enemy stand still when hit react happens
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	// my line in which the enemy doesn't stop but slows down, can change the division to variable if desired in future
	//GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? (BaseWalkSpeed/2) : BaseWalkSpeed;
}

void ABaseCharacter::HandleGameplayEffectRemoved(const FActiveGameplayEffectHandle InGEHandle)
{
	FOnActiveGameplayEffectRemoved_Info* DelPtr = AbilitySystemComponent->OnGameplayEffectRemoved_InfoDelegate(InGEHandle);
	if (DelPtr)
	{
		DelPtr->AddUObject(this, &ABaseCharacter::OnGameplayEffectRemoved);
	}
}

void ABaseCharacter::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& InGERemovalInfo)
{
	GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Green, TEXT("OnGERemoved"));
	UMyAbilitySystemComponent* HDASC = CastChecked<UMyAbilitySystemComponent>(AbilitySystemComponent);
	FGameplayTagContainer EffectTags;
	InGERemovalInfo.ActiveEffect->Spec.GetAllGrantedTags(EffectTags);
	TArray<FGameplayTag> GameplayTags;
	EffectTags.GetGameplayTagArray(GameplayTags);
	HDASC->MulticastActivatePassiveEffect(GameplayTags[0], false);
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass)
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	if (StaminaRegeneration)
	{
		ApplyEffectToSelf(StaminaRegeneration, 1.f);
	}
}

void ABaseCharacter::Die(const FVector& DeathImpulse, bool bRagdoll)
{
	MulticastHandleDeath(DeathImpulse, bRagdoll);
}

FOnDeath& ABaseCharacter::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void ABaseCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse, bool bRagdoll)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	if (bRagdoll)
	{
	
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

		GetCapsuleComponent()->DestroyComponent();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bDeadOnGround = true;
		OnDeathDelegate.Broadcast(this);
	}
	else
	{
		if (DeathMontage)
		{
			PlayAnimMontage(DeathMontage);
		}
	}
	bDead = true;
}


void ABaseCharacter::DeathMontageEnded()
{
	bDeadOnGround = true;
	OnDeathDelegate.Broadcast(this);
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

FOnDamageSignature& ABaseCharacter::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

FOnASCRegistered& ABaseCharacter::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ABaseCharacter::InitAbilityActorInfo()
{
	
}

