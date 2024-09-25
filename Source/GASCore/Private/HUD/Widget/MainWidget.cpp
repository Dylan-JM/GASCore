// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widget/MainWidget.h"

void UMainWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
