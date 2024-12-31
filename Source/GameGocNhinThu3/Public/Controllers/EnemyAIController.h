// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class IEnemyInterface;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void UpdatePatrolLocation();
	UFUNCTION(BlueprintCallable)
	void CheckDistance(AActor* AIACtor, AActor* PlayerActor, float AttackRange);


protected:
	virtual void OnPossess(APawn* InPawn) override;
	//FUCNTION
private:
	UFUNCTION()
	void HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	void HandleSeePlayer(AActor* Actor);

private:
	UPROPERTY()
	APawn* PossessedPawn;
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Sight* AIEnemySightConfig;
	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree;
	TScriptInterface<IEnemyInterface> EnemyInterface;

	UPROPERTY(EditDefaultsOnly)
	FName Key_PatrolLocation;
	UPROPERTY(EditDefaultsOnly)
	FName Key_IsCombat;
	UPROPERTY(EditDefaultsOnly)
	FName Key_PlayerActor;
	UPROPERTY(EditDefaultsOnly)
	FName Key_ShouldAttack;

	FLinearColor DebugColor = FLinearColor::Green;
};
