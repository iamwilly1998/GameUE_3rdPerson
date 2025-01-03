// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "Interfaces/AttackInterface.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "DataAssets/BaseCharacterData.h"
#include "Controllers/EnemyAIController.h"


void AEnemyCharacter::I_HandleAttackSuccess()
{
	Super::I_HandleAttackSuccess();
	if (AttackInterface_Player && StaminaComponent)
		AttackInterface_Player->I_HandleTargetAttacked(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
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

	// Reduce walk speed when combat
	if (BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->CombatSpeed);

	// attack interface
	// enter combat
	// health/max health
	AttackInterface_Player = TScriptInterface<IAttackInterface>(PlayerActor);

	if (AttackInterface_Player == nullptr)
		return;

	if (AttackInterface_Player && HealthComponent && StaminaComponent)
		AttackInterface_Player->I_EnterCombat(HealthComponent->Health, HealthComponent->MaxHealth, StaminaComponent->Stamina, StaminaComponent->MaxStamina);

	if (AttackInterface_Player->I_OnExitCombat.IsBound() == false)
		AttackInterface_Player->I_OnExitCombat.BindDynamic(this, &AEnemyCharacter::HandlePlayerExitCombat);
}

void AEnemyCharacter::Destroyed()
{
	if (AttackInterface_Player)
		AttackInterface_Player->I_HandleTargetDestroyed();
	Super::Destroyed();

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->PatrolSpeed);
}

void AEnemyCharacter::HandleTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	Super::HandleTakePointDamage(DamagedActor, Damage, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);
	if (AttackInterface_Player && HealthComponent)
		AttackInterface_Player->I_HitTarget(HealthComponent->Health, HealthComponent->MaxHealth);
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
	auto EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController)
		EnemyAIController->BackToPatrol();
}
