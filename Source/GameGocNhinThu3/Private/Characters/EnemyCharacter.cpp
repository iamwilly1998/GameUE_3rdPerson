// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "Interfaces/AttackInterface.h"
#include "Components/HealthComponent.h"
#include "DataAssets/BaseCharacterData.h"


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
	// attack interface
	// enter combat
	// health/max health
	AttackInterface_Player = TScriptInterface<IAttackInterface>(PlayerActor);
	if (AttackInterface_Player && HealthComponent)
		AttackInterface_Player->I_EnterCombat(HealthComponent->Health, HealthComponent->MaxHealth);
	// Reduce walk speed when combat
	if(BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->CombatSpeed);
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
