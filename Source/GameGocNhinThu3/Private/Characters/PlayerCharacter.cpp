// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Widgets/PlayerWidget.h"
#include "Components/HealthComponent.h"

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Playerwidget
	// World
	auto PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

	if (PlayerWidget && HealthComponent)
	{
		PlayerWidget->AddToViewport();
		PlayerWidget->UpdateHealthBar_Player(HealthComponent->Health / HealthComponent->MaxHealth);
	}


}
