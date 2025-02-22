// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/EnemyAIController.h"
#include "Interfaces/EnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Enum/AIState.h"


AEnemyAIController::AEnemyAIController()
{
	SetGenericTeamId(GetGenericTeamId());
	// AI Perception Component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	// Sight config
	AIEnemySightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Enemy Sight Config"));

	AIEnemySightConfig->SightRadius = 2500.0f;
	AIEnemySightConfig->LoseSightRadius = 2500.0f;
	AIEnemySightConfig->PeripheralVisionAngleDegrees = 55.0f;

	AIEnemySightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIEnemySightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	AIEnemySightConfig->DetectionByAffiliation.bDetectEnemies = true;

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
	CombatMode(Actor);

	if (EnemyInterface)
		EnemyInterface->I_HandleSeePlayer(Actor);
}


void AEnemyAIController::CombatMode(AActor* Actor)
{
	DebugColor = FLinearColor::Red;

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
	SetFocus(Actor);
}

void AEnemyAIController::UpdatePatrolLocation()
{
	if (Blackboard && EnemyInterface)
		Blackboard->SetValueAsVector(Key_PatrolLocation, EnemyInterface->I_GetPatrolLocation());
}

void AEnemyAIController::CheckDistance(AActor* AIACtor, AActor* PlayerActor, float AttackRange)
{
	if (PlayerActor == nullptr)
	{
		BackToPatrol();
		return;
	}

	if (bIsRegenStamina) return;

	if (AIACtor == nullptr) return;

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

	if (GetWorldTimerManager().IsTimerActive(ExitCombatTimer))
		GetWorldTimerManager().ClearTimer(ExitCombatTimer);

	GetWorldTimerManager().SetTimer(
		ExitCombatTimer, 
		this, 
		&AEnemyAIController::ExitCombatTimerFinished, 
		ExitCombatSecond);

	SetFocus(nullptr);
}

void AEnemyAIController::StartRegenStamina(float Stamina)
{
	bIsRegenStamina = true;

	TargetStamina = Stamina;

	if (Blackboard)
		Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Regen);
}

void AEnemyAIController::RegenToCombat()
{
	bIsRegenStamina = false;

	if (Blackboard)
		Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::Combat);
}

void AEnemyAIController::UpdateRegenLocation(AActor* AIACtor, AActor* PlayerActor, float RegenRange)
{
	// C = A + (h * distance)
	if (PlayerActor == nullptr || AIACtor == nullptr) return;

	const auto Direction_Player_AI = UKismetMathLibrary::GetDirectionUnitVector(PlayerActor->GetActorLocation(), AIACtor->GetActorLocation());

	const auto RegenLocation = PlayerActor->GetActorLocation() + (Direction_Player_AI * RegenRange);

	if(Blackboard)
		Blackboard->SetValueAsVector(Key_RegenLocation, RegenLocation);
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (APawn const* OtherPawn = Cast<APawn>(&Other))
	{
		if (auto const TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			if (TeamAgent->GetGenericTeamId() == GetGenericTeamId())
			{
				return ETeamAttitude::Friendly;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}
	return ETeamAttitude::Neutral;
}

FGenericTeamId AEnemyAIController::GetGenericTeamId() const
{
	return TeamId;
}

void AEnemyAIController::ExitCombatTimerFinished()
{
	DebugColor = FLinearColor::Green;

	if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound() == false)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdate);
	}
}
