// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class GAMEGOCNHINTHU3_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

private:
	void SpawnEnemy();

	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> PatrolPoints;

	FTimerHandle RespawnTimer;

	UPROPERTY(EditDefaultsOnly)
	float RespawnSecond = 3.0f;

};
