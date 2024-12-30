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
	// if not attack and can combo
	bool bCanAttack = bIsAttacking == false || bCanCombo;
	if (bCanAttack)
		Attack();
	else bSavedAttack = true;
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
	// print Bone name
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

UAnimMontage* UAttackComponent::GetCorrectAttackMontage()
{
	if (BaseCharacterData == nullptr)
		return nullptr;
	if (BaseCharacterData->AttackMontages.IsEmpty())
		return nullptr;
	return BaseCharacterData->AttackMontages[AttackIndex];
}

void UAttackComponent::Attack()
{
	// Attack Interface 
	if (AttackInterface && BaseCharacterData && GetCorrectAttackMontage())
	{
		AttackInterface->I_PlayAttackMontage(GetCorrectAttackMontage());
		// Play attacking sound
		AttackInterface->I_PlayAttackingSound();
		bIsAttacking = true;
		bCanCombo = false;
		AttackIndex = (AttackIndex +1) % BaseCharacterData->AttackMontages.Num();
	}
}

void UAttackComponent::SetupAttackComponent(UBaseCharacterData* BCD)
{
	BaseCharacterData = BCD;
}

void UAttackComponent::AN_EndAttack()
{
	bIsAttacking = false;
	bCanCombo = false;
	bSavedAttack = false;
	AttackIndex = 0;
}

void UAttackComponent::AN_ComboAttack()
{
	bCanCombo = true;
	if (bSavedAttack)
	{
		RequestAttack();
		bSavedAttack = false;
	}
}

void UAttackComponent::SetupTraceHit()
{
	HittedActors.Empty();
	//HitCount = 0;
}





