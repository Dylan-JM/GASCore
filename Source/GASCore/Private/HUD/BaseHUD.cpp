

#include "HUD/BaseHUD.h"
#include "HUD/Widget/MainWidget.h"
#include "HUD/WidgetController/OverlayWidgetController.h"
#include "Blueprint/UserWidget.h"

void ABaseHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ABaseHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(MainHUDClass, TEXT("MainHUD uninitialized, please fill out BP_BaseHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay widget controller class uninitialized, please filll out bp_basehud"));
	
	UUserWidget*  Widget = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
	MainHUDWidget = Cast<UMainWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	MainHUDWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}

void ABaseHUD::AddMainHUD()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && MainHUDClass)
	{
		MainHUDWidget = CreateWidget<UMainWidget>(PlayerController, MainHUDClass);
		MainHUDWidget->AddToViewport();
	}
}

UOverlayWidgetController* ABaseHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
			
}