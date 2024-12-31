// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UPlayerWidget;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputData;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Attack Interface
	virtual void I_EnterCombat(float Health_Enemy, float MaxHealth_Enemy) override;
	virtual void I_HitTarget(float Health_Target, float MaxHealth_Target) override;


protected:
	virtual void BeginPlay() override;


private:
	void CharacterAddMappingContext();
	void LookAround(const FInputActionValue& Value);
	void Moving(const FInputActionValue& Value);
	void AttackPressed();

private:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPlayerWidget* PlayerWidget;
};
