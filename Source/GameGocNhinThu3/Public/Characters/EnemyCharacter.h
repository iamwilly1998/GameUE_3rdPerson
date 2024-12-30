// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API AEnemyCharacter : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()
public:
	//Enemy Interface
	virtual FVector I_GetTargetLocation() override;
private:
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<AActor*> PatrolPoints;
	int PatrolIndex = 0;
};
