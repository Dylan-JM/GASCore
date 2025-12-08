// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/CoreWidget.h"

void UCoreWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
