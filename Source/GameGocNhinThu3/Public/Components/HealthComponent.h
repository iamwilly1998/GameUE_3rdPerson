// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UBaseCharacterData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEGOCNHINTHU3_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();
	void SetupComponent(UBaseCharacterData* BCD);
	void UpdateHealthByDamage(float Damage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	float Health = 0.0f;
	float MaxHealth = 100.0f;
		
};
