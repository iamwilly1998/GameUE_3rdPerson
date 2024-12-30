// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthBar_Player(float Health);
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar_Player;
};
