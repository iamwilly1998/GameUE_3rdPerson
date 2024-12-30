// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerWidget.h"
#include "Components/ProgressBar.h"

void UPlayerWidget::UpdateHealthBar_Player(float Health)
{
	// Progress bar
	if (HealthBar_Player)
		HealthBar_Player->SetPercent(Health);
}
