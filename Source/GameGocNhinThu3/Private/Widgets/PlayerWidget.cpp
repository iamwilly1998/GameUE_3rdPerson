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

void UPlayerWidget::UpdateStaminaBar_Player(float Stamina, float MaxStamina)
{
	if (StaminaBar_Player)
		StaminaBar_Player->SetPercent(Stamina / MaxStamina);
	FString StaminaString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Stamina), FMath::CeilToInt(MaxStamina));
	StaminaText_Player->SetText(FText::FromString(StaminaString));
}

void UPlayerWidget::UpdateStaminaBar_Enemy(float Stamina, float MaxStamina)
{
	if (StaminaBar_Enemy)
		StaminaBar_Enemy->SetPercent(Stamina / MaxStamina);
	FString StaminaString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Stamina), FMath::CeilToInt(MaxStamina));
	StaminaText_Enemy->SetText(FText::FromString(StaminaString));
}

void UPlayerWidget::ShowEnemyStats()
{
	if (HealthBar_Enemy && HealthText_Enemy && StaminaBar_Enemy && StaminaText_Enemy)
	{
		HealthBar_Enemy->SetVisibility(ESlateVisibility::Visible);
		HealthText_Enemy->SetVisibility(ESlateVisibility::Visible);
		StaminaBar_Enemy->SetVisibility(ESlateVisibility::Visible);
		StaminaText_Enemy->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerWidget::HideEnemyStats()
{
	if (HealthBar_Enemy && HealthText_Enemy && StaminaBar_Enemy && StaminaText_Enemy)
	{
		HealthBar_Enemy->SetVisibility(ESlateVisibility::Hidden);
		HealthText_Enemy->SetVisibility(ESlateVisibility::Hidden);
		StaminaBar_Enemy->SetVisibility(ESlateVisibility::Hidden);
		StaminaText_Enemy->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerWidget::UpdateKills(int Kill)
{
	FString KillString = FString::Printf(TEXT("%d"),Kill);
	if (Kills_Point)
		Kills_Point->SetText(FText::FromString(KillString));
}
