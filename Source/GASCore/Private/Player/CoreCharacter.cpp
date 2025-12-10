
#include "Player/CoreCharacter.h"
#include "Engine.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CoreGameplayTags.h"
#include "AbilitySystem/Data/StartupAbilities.h"
#include "AbilitySystem/Debuffs/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "AI/Enemy/CoreEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCore/GASCore.h"
#include "Net/UnrealNetwork.h"


ACoreCharacter::ACoreCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

void ACoreCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void ACoreCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoreCharacter, bDeadOnGround);
	DOREPLIFETIME(ACoreCharacter, bDead);
}

float ACoreCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

void ACoreCharacter::AddCharacterAbilities()
{
	UCoreAbilitySystemComponent* MASC = Cast<UCoreAbilitySystemComponent>(AbilitySystemComponent);
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

void ACoreCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	// this line makes it so the enemy stand still when hit react happens
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	// my line in which the enemy doesn't stop but slows down, can change the division to variable if desired in future
	//GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? (BaseWalkSpeed/2) : BaseWalkSpeed;
}

void ACoreCharacter::HandleGameplayEffectRemoved(const FActiveGameplayEffectHandle InGEHandle)
{
	FOnActiveGameplayEffectRemoved_Info* DelPtr = AbilitySystemComponent->OnGameplayEffectRemoved_InfoDelegate(InGEHandle);
	if (DelPtr)
	{
		DelPtr->AddUObject(this, &ACoreCharacter::OnGameplayEffectRemoved);
	}
}

void ACoreCharacter::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& InGERemovalInfo)
{
	GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Green, TEXT("OnGERemoved"));
	UCoreAbilitySystemComponent* CoreASC = CastChecked<UCoreAbilitySystemComponent>(AbilitySystemComponent);
	FGameplayTagContainer EffectTags;
	InGERemovalInfo.ActiveEffect->Spec.GetAllGrantedTags(EffectTags);
	TArray<FGameplayTag> GameplayTags;
	EffectTags.GetGameplayTagArray(GameplayTags);
	CoreASC->MulticastActivatePassiveEffect(GameplayTags[0], false);
}

void ACoreCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass)
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ACoreCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	if (StaminaRegeneration)
	{
		ApplyEffectToSelf(StaminaRegeneration, 1.f);
	}
}

void ACoreCharacter::Die(const FVector& DeathImpulse, bool bRagdoll)
{
	MulticastHandleDeath(DeathImpulse, bRagdoll);
}

FOnDeath& ACoreCharacter::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void ACoreCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse, bool bRagdoll)
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


void ACoreCharacter::DeathMontageEnded()
{
	bDeadOnGround = true;
	OnDeathDelegate.Broadcast(this);
}

bool ACoreCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ACoreCharacter::GetAvatar_Implementation()
{
	return this;
}

FOnDamageSignature& ACoreCharacter::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

ECharacterClass ACoreCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& ACoreCharacter::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

UAbilitySystemComponent* ACoreCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACoreCharacter::InitAbilityActorInfo()
{
	
}

