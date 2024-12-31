// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/EnemyAIController.h"
#include "Interfaces/EnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enum/AIState.h"


AEnemyAIController::AEnemyAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	// Sight config
	AIEnemySightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Enemy Sight Config"));
	AIEnemySightConfig->SightRadius = 2500.0f;
	AIEnemySightConfig->LoseSightRadius = 2500.0f;
	AIEnemySightConfig->PeripheralVisionAngleDegrees = 55.0f;
	AIEnemySightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*AIEnemySightConfig);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PossessedPawn = InPawn;
	EnemyInterface = TScriptInterface<IEnemyInterface>(InPawn);

	RunBehaviorTree(BehaviorTree);

	if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound() == false)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdate);
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (PossessedPawn == nullptr) return;
	UKismetSystemLibrary::DrawDebugCone(
		this, 
		PossessedPawn->GetActorLocation(),
		PossessedPawn->GetActorForwardVector(),
		2500.0f,
		FMath::DegreesToRadians(55.0f),
		FMath::DegreesToRadians(55.0f),
		20,
		DebugColor,
		-1.0f,
		3.0f
	);
}

void AEnemyAIController::HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		HandleSeePlayer(Actor);
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("I forgot you"));
		DebugColor = FLinearColor::Green;
	}
}

void AEnemyAIController::HandleSeePlayer(AActor* Actor)
{
	DebugColor = FLinearColor::Red;
	if (EnemyInterface)
		EnemyInterface->I_HandleSeePlayer(Actor);

	if (Blackboard)
	{
		// is combat, bool
		Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Combat);
		// player actor
		Blackboard->SetValueAsObject(Key_PlayerActor, Actor);
	}
	// Remove Dynamic
	if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound())
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdate);
	}
	
}

void AEnemyAIController::UpdatePatrolLocation()
{
	if (Blackboard && EnemyInterface)
		Blackboard->SetValueAsVector(
			Key_PatrolLocation
			, EnemyInterface->I_GetTargetLocation());
}

void AEnemyAIController::CheckDistance(AActor* AIACtor, AActor* PlayerActor, float AttackRange)
{
	if (PlayerActor == nullptr)
	{
		BackToPatrol();
		return;
	}

	if (AIACtor == nullptr || PlayerActor == nullptr)
		return;
	const auto Distance_AIVsPlayer = AIACtor->GetDistanceTo(PlayerActor);
	if (Distance_AIVsPlayer <= AttackRange)
	{
		if(Blackboard)
			Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Attack);
		}
		else
	{
		if (Blackboard)
			Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Combat);
	}
}

void AEnemyAIController::BackToPatrol()
{
	if (Blackboard)
		Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Patrol);
	DebugColor = FLinearColor::Gray;

	GetWorldTimerManager().SetTimer(
		ExitCombatTimer, 
		this, 
		&AEnemyAIController::ExitCombatTimerFinished, 
		ExitCombatSecond);
}

void AEnemyAIController::ExitCombatTimerFinished()
{
	DebugColor = FLinearColor::Green;
	if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound() == false)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdate);
	}
}
