// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EndWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UEndWidget::UpdateResultText(FText Text)
{
	if (ResultText)
		ResultText->SetText(Text);
}

bool UEndWidget::Initialize()
{
	if(Super::Initialize() == false)
		return false;
	if(PlayAgainButton)
		PlayAgainButton->OnClicked.AddDynamic(this, &UEndWidget::PlayAgainClicked);
	if(ExitButton)
		ExitButton->OnClicked.AddDynamic(this, &UEndWidget::ExitClicked);
	return true;
}

void UEndWidget::PlayAgainClicked()
{
	if (GetOwningPlayer() == nullptr) return;

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	FInputModeGameOnly MyInputMode;

	GetOwningPlayer()->SetInputMode(MyInputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);

	auto LevelString = UGameplayStatics::GetCurrentLevelName(GetWorld());
	auto LevelName = FName(*LevelString);
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void UEndWidget::ExitClicked()
{
	TEnumAsByte<EQuitPreference::Type> ExitPreference = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		UGameplayStatics::GetPlayerController(GetWorld(),0),
		ExitPreference, true
	);
}
