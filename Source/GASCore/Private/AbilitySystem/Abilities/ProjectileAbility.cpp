// Copyright DM


#include "AbilitySystem/Abilities/ProjectileAbility.h"
#include "AbilitySystemComponent.h"
#include "Actor/MyProjectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/PlayerCharacter.h"
#include "Player/BasePlayerController.h"

void UProjectileAbility::SpawnProjectile()
{
	
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		FRotator TargetRotation;
		FTransform OutTransform;
		FVector ToTarget;
		class USkeletalMeshSocket const* TipSocket = PlayerCharacter->GetMesh()->GetSocketByName("TestSocket");
		OutTransform = TipSocket->GetSocketTransform(PlayerCharacter->GetMesh());
		
		ABasePlayerController* PlayerController = Cast<ABasePlayerController>(PlayerCharacter->GetController());

		HitTargetLoc = PlayerCharacter->GetTraceResult(TraceDistance).ImpactPoint;
		ToTarget = HitTargetLoc - OutTransform.GetLocation();
		TargetRotation = ToTarget.Rotation();

		if (PlayerCharacter->IsLocallyControlled())
		{
			ServerSpawnProjectile(OutTransform, TargetRotation);
		}
	}
}

void UProjectileAbility::ServerSpawnProjectile_Implementation(FTransform OutTransform, FRotator TargetRotation)
{
	TObjectPtr<AMyProjectile> SpawnedProjectile = nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	
	SpawnedProjectile = GetWorld()->SpawnActor<AMyProjectile>(ProjectileClass, OutTransform.GetLocation(), TargetRotation, SpawnParams);
	
	SpawnedProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
}
