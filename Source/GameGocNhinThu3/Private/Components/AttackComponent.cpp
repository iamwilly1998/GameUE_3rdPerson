// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttackComponent.h"
#include "GameFramework/Character.h"
#include "DataAssets/BaseCharacterData.h"
#include "Interfaces/AttackInterface.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	//Character = Cast<ACharacter>(GetOwner());
	// Owner Actor -> Attack Interface
	AttackInterface = TScriptInterface<IAttackInterface>(GetOwner());
}

void UAttackComponent::RequestAttack()
{
	if (bIsAttacking)
		return;
	Attack();
}

void UAttackComponent::TraceHit()
{
	if (BaseCharacterData == nullptr)
		return;
	/*if (GetMesh() == nullptr)
		return;*/
	if (AttackInterface == nullptr)
		return;
	const FVector& StartLocation = AttackInterface->I_GetSocketLocation(BaseCharacterData->TraceStart);
	const FVector& EndLocation = AttackInterface->I_GetSocketLocation(BaseCharacterData->TraceEnd);
	/*const FVector& StartLocation = GetMesh()->GetSocketLocation(BaseCharacterData->TraceStart);
	const FVector& EndLocation = GetMesh()->GetSocketLocation(BaseCharacterData->TraceEnd);*/
	// Hit result
	TArray<FHitResult> HitResults;

	auto DrawDebug = BaseCharacterData->bDrawBugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bDoHitSomething = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		StartLocation,
		EndLocation,
		10.0f,
		BaseCharacterData->TraceObjectTypes,
		false,
		BaseCharacterData->ActorToIgnore,
		DrawDebug,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		4.0f
	);
	if (bDoHitSomething == false)
		return;


	for (const FHitResult& Result : HitResults)
	{
		if (HittedActors.Contains(Result.GetActor()))
			continue;

		HandleHitResult(Result);
		//UKismetSystemLibrary::DrawDebugSphere(this, Result.ImpactPoint, 10.0f);

		HittedActors.Emplace(Result.GetActor());
		//HitCount++;
	}
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Red,
			FString::Printf(TEXT("Hit Count = %d"), HitCount)
		);*/
}

void UAttackComponent::HandleHitResult(const FHitResult& Result)
{
	// print string
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Cyan,
			Result.BoneName.ToString()
		);
	// Character must sign in Delegate
	// Noti to character that it hitted target, use delegate
	// Character -> Apply Point Damage
	if(HitSomeThingDelegate.IsBound())
		HitSomeThingDelegate.Execute(Result);

}

void UAttackComponent::Attack()
{
	// Attack Interface 
	if (AttackInterface && BaseCharacterData)
		AttackInterface->I_PlayAttackMontage(BaseCharacterData->AttackMontage);
	bIsAttacking = true;
}

void UAttackComponent::SetupAttackComponent(UBaseCharacterData* BCD)
{
	BaseCharacterData = BCD;
}

void UAttackComponent::AN_EndAttack()
{
	bIsAttacking = false;
}

void UAttackComponent::SetupTraceHit()
{
	HittedActors.Empty();
	//HitCount = 0;
}




