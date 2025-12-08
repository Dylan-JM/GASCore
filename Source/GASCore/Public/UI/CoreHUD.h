// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CoreHUD.generated.h"


class USpellMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UMainWidget;

UCLASS()
class GASCORE_API ACoreHUD : public AHUD
{
	GENERATED_BODY()


public:

	
	virtual void DrawHUD() override;

	/* Player Overlay */
	void AddMainWidget();
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	
protected:


private:
	UPROPERTY()
	TObjectPtr<UMainWidget> MainWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
	
};
