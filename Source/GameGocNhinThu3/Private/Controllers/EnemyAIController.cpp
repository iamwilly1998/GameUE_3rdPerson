// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/EnemyAIController.h"
#include "Interfaces/EnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"


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

	//RunBehaviorTree(BehaviorTree);
	if (AIPerceptionComponent)
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdate);
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
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("I see you"));
		DebugColor = FLinearColor::Red;
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("I forgot you"));
		DebugColor = FLinearColor::Green;
	}
}


void AEnemyAIController::UpdatePatrolLocation()
{
	if (Blackboard && EnemyInterface)
		Blackboard->SetValueAsVector(
			Key_PatrolLocation
			, EnemyInterface->I_GetTargetLocation());
}