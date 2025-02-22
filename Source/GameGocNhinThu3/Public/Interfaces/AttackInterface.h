// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class GAMEGOCNHINTHU3_API IAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void I_PlayAttackMontage(UAnimMontage* AttackMontage) = 0;
	virtual void I_PlayAttackingSound() = 0;

	virtual void I_AN_EndAttack() = 0; 
	virtual void I_AN_ComboAttack() = 0;
	virtual void I_AN_EndHitReact() = 0;

	// Getter
	virtual FVector I_GetSocketLocation(const FName& SocketName) const = 0;


	virtual void I_ANS_TraceHit() = 0;
	virtual void I_ANS_BeginTraceHit() = 0;

	virtual void I_EnterCombat(AActor* TargetActor) = 0;
	virtual void I_ReceiveCombat(AActor* TargetActor) = 0;
	virtual void I_ExitCombat() = 0;

	virtual void I_HandleExitCombat() = 0;

	virtual void I_HitTarget(float Health_Target, float MaxHealth_Target);

	UFUNCTION(BlueprintCallable)
	virtual void I_RequestAttack() = 0;

	virtual void I_HandleTargetDestroyed();

	virtual void I_HandleAttackSuccess() = 0;

	virtual void I_HandleTargetAttacked(float Stamina_Target, float MaxStamina_Target);

	virtual bool I_DoesReadyToAttack() const = 0;

	virtual bool I_HasEnoughStamina(float Cost) const = 0;

	virtual void I_RegenStamina();

	virtual void I_Target_RegenStamina(float Stamina_Target, float MaxStamina_Target);

	virtual bool I_IsInCombat() const = 0;

	virtual void I_RequestAttackFail_Stamina(float StaminaCost);

	// Stats
	virtual float I_GetHealth() const = 0;
	virtual float I_GetMaxHealth() const = 0;
	virtual float I_GetStamina() const = 0;
	virtual float I_GetMaxStamina() const = 0;
};
