// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASCORE_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributesPointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAbilityPointsReward(int32 Level) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToPlayerLevel(int32 InPlayerLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAbilityPoints() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToAbilityPoints(int32 InAbilityPoints);
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	
};
