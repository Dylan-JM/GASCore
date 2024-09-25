

#include "Player/BasePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "InputActionValue.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/BasePlayerState.h"
#include "UI/BaseHUD.h"
#include "UI/Widget/DamageTextComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"

ABasePlayerController::ABasePlayerController()
{
	bReplicates = true;
}

void ABasePlayerController::BeginPlay()
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
	BaseHUD = Cast<ABaseHUD>(GetHUD());
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* HDInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	HDInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
	HDInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);
	
}

void ABasePlayerController::Move(const FInputActionValue& Value)
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

void ABasePlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
		PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
	}
}

void ABasePlayerController::SprintButtonPressed()
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

void ABasePlayerController::SprintButtonReleased()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetSprinting(false);
	}
}

void ABasePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
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

UMyAbilitySystemComponent* ABasePlayerController::GetASC()
{
	if (HDAbilitySystemComponent == nullptr)
	{
		HDAbilitySystemComponent = Cast<UMyAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return HDAbilitySystemComponent;
}
