// Copyright DM

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "ProjectileAbility.generated.h"


class AProjectile;
class UGameplayEffect;

UCLASS()
class GASCORE_API UProjectileAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()



protected:

	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectile();

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(FTransform OutTransform, FRotator TargetRotation);

	UPROPERTY(BlueprintReadWrite)
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;

	UPROPERTY(BlueprintReadOnly)
	FVector HitTargetLoc;
};
