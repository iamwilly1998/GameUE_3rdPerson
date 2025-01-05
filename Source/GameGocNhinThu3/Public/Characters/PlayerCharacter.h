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
	virtual void I_EnterCombat(AActor* TargetActor) override;
	virtual void I_ExitCombat() override;
	virtual void I_HitTarget(float Health_Target, float MaxHealth_Target) override;
	virtual void I_HandleTargetDestroyed() override;
	virtual void I_HandleAttackSuccess() override;
	virtual void I_HandleTargetAttacked(float Stamina_Target, float MaxStamina_Target) override;
	virtual void I_RegenStamina() override;
	virtual void I_Target_RegenStamina(float Stamina_Target, float MaxStamina_Target) override;


protected:
	virtual void BeginPlay() override;
	virtual void HandleTakePointDamage(
		AActor* DamagedActor,
		float Damage,
		class AController* InstigatedBy,
		FVector HitLocation,
		class UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const class UDamageType* DamageType,
		AActor* DamageCauser) override;

	virtual void HandleDead() override;

private:
	void CharacterAddMappingContext();
	void LookAround(const FInputActionValue& Value);
	void Moving(const FInputActionValue& Value);
	void AttackPressed();
	void StrongAttackPressed();
	void ExitCombatPressed();
	void ShowTargetStats();

private:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPlayerWidget* PlayerWidget;
};
