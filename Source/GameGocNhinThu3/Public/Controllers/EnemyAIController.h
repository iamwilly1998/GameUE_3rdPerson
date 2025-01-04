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
	void BackToPatrol();
	void StartRegenStamina(float Stamina);
	void RegenToCombat();
	UFUNCTION(BlueprintCallable)
	void UpdateRegenLocation(AActor* AIACtor, AActor* PlayerActor, float RegenRange);


protected:
	virtual void OnPossess(APawn* InPawn) override;
	//FUCNTION
private:
	UFUNCTION()
	void HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	void HandleSeePlayer(AActor* Actor);

	void ExitCombatTimerFinished();

public:
	bool bIsRegenStamina = false;
	float TargetStamina = 0.0f;

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
	FName Key_PlayerActor;
	UPROPERTY(EditDefaultsOnly)
	FName Key_AIState;
	UPROPERTY(EditDefaultsOnly)
	FName Key_RegenLocation;

	FLinearColor DebugColor = FLinearColor::Green;

	FTimerHandle ExitCombatTimer;

	UPROPERTY(EditDefaultsOnly)
	float ExitCombatSecond = 3.0f;

};
