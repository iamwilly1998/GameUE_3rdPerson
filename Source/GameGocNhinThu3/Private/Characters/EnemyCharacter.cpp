// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "Interfaces/AttackInterface.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/AttackComponent.h"
#include "DataAssets/BaseCharacterData.h"
#include "Controllers/EnemyAIController.h"



void AEnemyCharacter::I_HandleAttackSuccess()
{
	Super::I_HandleAttackSuccess();
	if (AttackInterface_Target && StaminaComponent)
		AttackInterface_Target->I_HandleTargetAttacked(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
}

void AEnemyCharacter::I_RegenStamina()
{
	if(AttackInterface_Target && StaminaComponent)
		AttackInterface_Target->I_Target_RegenStamina(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
	if (EnemyAIController == nullptr)
		return;
	if (EnemyAIController->bIsRegenStamina)
	{
		if (I_HasEnoughStamina(EnemyAIController->TargetStamina))
			EnemyAIController->RegenToCombat();
	}
}

void AEnemyCharacter::I_RequestAttackFail_Stamina(float StaminaCost)
{
	if (EnemyAIController)
		EnemyAIController->StartRegenStamina(StaminaCost);
}

FVector AEnemyCharacter::I_GetTargetLocation()
{
	if(PatrolPoints.IsEmpty())
		return GetActorLocation();
	if (PatrolPoints[PatrolIndex] == nullptr)
		return GetActorLocation();
	auto ReturnValue = PatrolPoints[PatrolIndex]->GetActorLocation();
	PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	return ReturnValue;
}

void AEnemyCharacter::I_HandleSeePlayer(AActor* PlayerActor)
{
	I_EnterCombat(PlayerActor);
}

void AEnemyCharacter::Destroyed()
{
	if (AttackInterface_Target)
		AttackInterface_Target->I_HandleTargetDestroyed();
	Super::Destroyed();

}

void AEnemyCharacter::I_EnterCombat(AActor* TargetActor)
{
	Super::I_EnterCombat(TargetActor);

	// attack interface
	// enter combat
	// health/max health

	if (AttackInterface_Target == nullptr)
		return;
	AttackInterface_Target->I_EnterCombat(this);
}

void AEnemyCharacter::I_HandleExitCombat()
{
	Super::I_HandleExitCombat();
	if (EnemyAIController)
		EnemyAIController->BackToPatrol();
}

void AEnemyCharacter::I_RequestAttack()
{
	if (AttackComponent == nullptr)
		return;
	if (AttackComponent->AttackCount >= 3)
	{
		AttackComponent->RequestAttackType = EAttackType::StrongAttack;
	}
	else {
		AttackComponent->RequestAttackType = EAttackType::LightAttack;
	}
	
	Super::I_RequestAttack();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->PatrolSpeed);
	EnemyAIController = Cast<AEnemyAIController>(GetController());

}

void AEnemyCharacter::HandleTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	Super::HandleTakePointDamage(DamagedActor, Damage, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);
	if (AttackInterface_Target && HealthComponent)
		AttackInterface_Target->I_HitTarget(HealthComponent->Health, HealthComponent->MaxHealth);
}

void AEnemyCharacter::HandleDead()
{
	Super::HandleDead();
	// Stop behavior tree
	// Remove controller
	DetachFromControllerPendingDestroy();
}

void AEnemyCharacter::HandlePlayerExitCombat()
{
	// change to patrol
	// AI state -> patrol
	if (EnemyAIController)
		EnemyAIController->BackToPatrol();
}
