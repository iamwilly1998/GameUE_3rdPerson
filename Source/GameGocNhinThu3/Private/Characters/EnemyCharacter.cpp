// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"

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
