// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CorePlayerCharacter.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CorePlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/CorePlayerState.h"
#include "UI/CoreHUD.h"


ACorePlayerCharacter::ACorePlayerCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->SetAutoActivate(false);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f);

}

void ACorePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GetTraceResult(DefaultTraceDistance);
	
}

void ACorePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACorePlayerCharacter, MovementDirectionVector);
}

void ACorePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacterController = Cast<ACorePlayerController>(GetController());
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void ACorePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (PlayerCharacterController == nullptr) PlayerCharacterController = Cast<ACorePlayerController>(NewController);
	
	// Init ability actor info for the Server
	InitAbilityActorInfo();

	
	InitializeDefaultAttributes();
	AddCharacterAbilities();
	
}

void ACorePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	
}

void ACorePlayerCharacter::InitAbilityActorInfo()
{
	ACorePlayerState* PS = GetPlayerState<ACorePlayerState>();
	if (!IsValid(PS))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,"Player state is not valid");
		return;
	}
	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	Cast<UCoreAbilitySystemComponent>(PS->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSetBase = PS->GetAttributeSetBase();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	
	if (ACorePlayerController* PlayerController = Cast<ACorePlayerController>(GetController()))
	{
		if (ACoreHUD* BaseHUD = Cast<ACoreHUD>(PlayerController->GetHUD()))
		{
			BaseHUD->InitOverlay(PlayerController, PS, AbilitySystemComponent, AttributeSetBase);
		}
	}
}



void ACorePlayerCharacter::Jump()
{
	Super::Jump();
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ACorePlayerCharacter::TraceUnderCrosshairs(FHitResult& TraceHitResult, float TraceDistance, bool bUseVisibilityChannel)
{
	if (!IsValid(PlayerCharacterController)) return;
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	ECollisionChannel TraceChannel = ECC_Visibility;;

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		
		// This is for if I want the thing to fire straight forward FVector EndLocation = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, CrosshairWorldDirection.Z);
		FVector End = Start + CrosshairWorldDirection * TraceDistance;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, TraceChannel);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

FHitResult ACorePlayerCharacter::GetTraceResult(float TraceDistance, bool bUseVisibilityChannel)
{
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult, TraceDistance, bUseVisibilityChannel);
	return HitResult;
}

void ACorePlayerCharacter::SetSprinting(bool bInIsSprinting)
{
	if (bIsSprinting == bInIsSprinting) return;
	bIsSprinting = bInIsSprinting;
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
	else if (!bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
	ServerSetSprinting(bInIsSprinting);
}

void ACorePlayerCharacter::ServerSetSprinting_Implementation(bool bInIsSprinting)
{
	bIsSprinting = bInIsSprinting;
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
	else if (!bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void ACorePlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	IPlayerInterface::AddToXP_Implementation(InXP);
}

void ACorePlayerCharacter::LevelUp_Implementation()
{
	IPlayerInterface::LevelUp_Implementation();
}

int32 ACorePlayerCharacter::GetXP_Implementation() const
{
	return IPlayerInterface::GetXP_Implementation();
}

int32 ACorePlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	return IPlayerInterface::FindLevelForXP_Implementation(InXP);
}

int32 ACorePlayerCharacter::GetAttributesPointsReward_Implementation(int32 Level) const
{
	return IPlayerInterface::GetAttributesPointsReward_Implementation(Level);
}

int32 ACorePlayerCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	return IPlayerInterface::GetSpellPointsReward_Implementation(Level);
}

void ACorePlayerCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	ACorePlayerState* PS = GetPlayerState<ACorePlayerState>();
	check(PS);
	PS->AddToLevel(InPlayerLevel);

	if (UCoreAbilitySystemComponent* CoreASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		CoreASC->UpdateAbilityStatuses(PS->GetPlayerLevel());
	}
}

void ACorePlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	IPlayerInterface::AddToAttributePoints_Implementation(InAttributePoints);
}

void ACorePlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	IPlayerInterface::AddToSpellPoints_Implementation(InSpellPoints);
}

int32 ACorePlayerCharacter::GetAttributePoints_Implementation() const
{
	return IPlayerInterface::GetAttributePoints_Implementation();
}

int32 ACorePlayerCharacter::GetSpellPoints_Implementation() const
{
	return IPlayerInterface::GetSpellPoints_Implementation();
}