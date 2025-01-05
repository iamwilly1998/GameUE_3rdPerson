// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

class IAttackInterface;
class AEnemyAIController;
/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API AEnemyCharacter : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Attack Interface
	virtual void I_EnterCombat(AActor* TargetActor) override;
	virtual void I_HandleExitCombat() override;

	virtual void I_RequestAttack() override;
	virtual void I_HandleAttackSuccess();
	virtual void I_RegenStamina() override;
	virtual void I_RequestAttackFail_Stamina(float StaminaCost) override;

	//Enemy Interface
	virtual FVector I_GetTargetLocation() override;
	virtual void I_HandleSeePlayer(AActor* PlayerActor) override;
	virtual void Destroyed() override;

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
	UFUNCTION()
	void HandlePlayerExitCombat();

private:

	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<AActor*> PatrolPoints;
	int PatrolIndex = 0;

	UPROPERTY()
	AEnemyAIController* EnemyAIController;
};
