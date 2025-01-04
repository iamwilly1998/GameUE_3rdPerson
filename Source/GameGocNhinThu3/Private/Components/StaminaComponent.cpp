// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StaminaComponent.h"
#include "DataAssets/BaseCharacterData.h"
#include "Interfaces/AttackInterface.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetComponentTickInterval(0.03f);
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	AttackInterface = TScriptInterface<IAttackInterface>(GetOwner());
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Regen Stamina

	if (Stamina < MaxStamina && AttackInterface && AttackInterface->I_IsInCombat() == false)
	{
		Stamina = FMath::Min(Stamina + (RegenStaminaRate * DeltaTime), MaxStamina);
		AttackInterface->I_RegenStamina();
	}
}

void UStaminaComponent::SetupComponent(UBaseCharacterData* BCD)
{
	if (BCD)
	{
		Stamina = BCD->Stamina;
		MaxStamina = BCD->MaxStamina;
		RegenStaminaRate = BCD->RegenStaminaRate;
	}
}

void UStaminaComponent::UpdateStamina(float Cost)
{
	Stamina = FMath::Max(Stamina - Cost, 0.0f);
}

