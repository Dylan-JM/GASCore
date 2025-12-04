
#include "Player/PlayerAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "Player/CoreCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BaseCharacter = Cast<ACoreCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character == nullptr)
	{
		Character = Cast<ACharacter>(TryGetPawnOwner());
	}
	if (Character == nullptr) return;

	FVector Velocity = Character->GetMovementComponent()->Velocity;
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, GetOwningActor()->GetActorRotation());
	bIsInAir = Character->GetCharacterMovement()->IsFalling();
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true:false;
	
	if (BaseCharacter)
	{
		bIsDead = BaseCharacter->bDead;
	}
	
	// Offset Yaw for strafing
	
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetBaseAimRotation(), Character->GetActorRotation());
	DeltaYaw= DeltaRotator.Yaw;
	DeltaPitch = DeltaRotator.Pitch;

	/*DeltaRoll = DeltaRotator.Roll;*/
}
