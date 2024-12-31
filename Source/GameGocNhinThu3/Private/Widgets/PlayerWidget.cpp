// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerWidget::UpdateHealthBar_Player(float Health, float MaxHealth)
{
	// Progress bar
	if (HealthBar_Player)
		HealthBar_Player->SetPercent(Health / MaxHealth);
	// Update UTextBlock
	FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	HealthText_Player->SetText(FText::FromString(HealthString));
}

void UPlayerWidget::UpdateHealthBar_Enemy(float Health, float MaxHealth)
{
	// Progress bar
	if (HealthBar_Enemy)
		HealthBar_Enemy->SetPercent(Health / MaxHealth);
	// Update UTextBlock
	FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	HealthText_Enemy->SetText(FText::FromString(HealthString));
}

void UPlayerWidget::ShowEnemyStats()
{
	if (HealthBar_Enemy && HealthText_Enemy)
	{
		HealthBar_Enemy->SetVisibility(ESlateVisibility::Visible);
		HealthText_Enemy->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerWidget::HideEnemyStats()
{
	if (HealthBar_Enemy && HealthText_Enemy)
	{
		HealthBar_Enemy->SetVisibility(ESlateVisibility::Hidden);
		HealthText_Enemy->SetVisibility(ESlateVisibility::Hidden);
	}
}
