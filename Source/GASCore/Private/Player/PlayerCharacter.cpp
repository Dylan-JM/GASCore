// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "AbilitySystem/CoreGameplayTags.h"
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


APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GetTraceResult(DefaultTraceDistance);
	
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, MovementDirectionVector);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacterController = Cast<ACorePlayerController>(GetController());
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (PlayerCharacterController == nullptr) PlayerCharacterController = Cast<ACorePlayerController>(NewController);
	
	// Init ability actor info for the Server
	InitAbilityActorInfo();

	
	InitializeDefaultAttributes();
	AddCharacterAbilities();
	
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	
}

void APlayerCharacter::InitAbilityActorInfo()
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

void APlayerCharacter::Jump()
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

void APlayerCharacter::TraceUnderCrosshairs(FHitResult& TraceHitResult, float TraceDistance, bool bUseVisibilityChannel)
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

FHitResult APlayerCharacter::GetTraceResult(float TraceDistance, bool bUseVisibilityChannel)
{
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult, TraceDistance, bUseVisibilityChannel);
	return HitResult;
}

void APlayerCharacter::SetSprinting(bool bInIsSprinting)
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

void APlayerCharacter::ServerSetSprinting_Implementation(bool bInIsSprinting)
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