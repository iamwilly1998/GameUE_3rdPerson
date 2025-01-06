// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndWidget.generated.h"
   
class UTextBlock;
class UButton;

UCLASS()
class GAMEGOCNHINTHU3_API UEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateResultText(FText Text);
	virtual bool Initialize() override;
private:
	UFUNCTION()
	void PlayAgainClicked();
	UFUNCTION()
	void ExitClicked();
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;
	UPROPERTY(meta = (BindWidget))
	UButton* PlayAgainButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
