

#include "Player/CorePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CoreGameplayTags.h"
#include "InputActionValue.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/CorePlayerState.h"
#include "UI/CoreHUD.h"
#include "UI/Widget/DamageTextComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"

ACorePlayerController::ACorePlayerController()
{
	bReplicates = true;
}

void ACorePlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(PlayerMappingContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(PlayerMappingContext, 0);
		if (!Subsystem->GetUserSettings()->IsMappingContextRegistered(PlayerMappingContext))
		{
			Subsystem->GetUserSettings()->RegisterInputMappingContext(PlayerMappingContext);
		}
	}
	BaseHUD = Cast<ACoreHUD>(GetHUD());
}

void ACorePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ACorePlayerState* PS = GetPlayerState<ACorePlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ACorePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* HDInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	HDInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ACorePlayerController::Move);
	HDInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACorePlayerController::Look);
	
}

void ACorePlayerController::Move(const FInputActionValue& Value)
{
	if (PlayerCharacter && PlayerCharacter->bDead) return;
	if (GetASC() && GetASC()->HasMatchingGameplayTag(GasTag::Player_Block_InputPressed))
	{
		return;
	}
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (PlayerCharacter == nullptr) PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	
	if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		PlayerCharacter->AddMovementInput(RightDirection, MovementVector.X);
		PlayerCharacter->MovementDirectionVector = MovementVector;
	}
}

void ACorePlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
		PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
	}
}

void ACorePlayerController::SprintButtonPressed()
{
	if (PlayerCharacter && PlayerCharacter->bDead) return;
	if (PlayerCharacter)
	{
		PlayerCharacter->SetSprinting(true);
		if (PlayerCharacter->bIsCrouched)
		{
			PlayerCharacter->UnCrouch(true);
		}
	}
}

void ACorePlayerController::SprintButtonReleased()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetSprinting(false);
	}
}

void ACorePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

UCoreAbilitySystemComponent* ACorePlayerController::GetASC()
{
	if (HDAbilitySystemComponent == nullptr)
	{
		HDAbilitySystemComponent = Cast<UCoreAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return HDAbilitySystemComponent;
}
