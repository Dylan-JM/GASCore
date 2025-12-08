

#include "UI/CoreHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/MainWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void ACoreHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ACoreHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(MainWidgetClass, TEXT("MainWidget uninitialized, please fill out BP_CoreHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay widget controller class uninitialized, please fill out BP_CoreHUD"));
	
	UUserWidget*  Widget = CreateWidget<UUserWidget>(GetWorld(), MainWidgetClass);
	MainWidget = Cast<UMainWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	MainWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}

void ACoreHUD::AddMainWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && MainWidgetClass)
	{
		MainWidget = CreateWidget<UMainWidget>(PlayerController, MainWidgetClass);
		MainWidget->AddToViewport();
	}
}

UOverlayWidgetController* ACoreHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
			
}