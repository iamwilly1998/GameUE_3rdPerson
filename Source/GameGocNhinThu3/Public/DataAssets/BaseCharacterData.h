// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseCharacterData.generated.h"

/**
 * 
 */
UCLASS()
class GAMEGOCNHINTHU3_API UBaseCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage;
	// Trace hit
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	FName TraceStart;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	FName TraceEnd;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	bool bDrawBugTrace = true;

	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	TArray<AActor*> ActorToIgnore;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Damage = 20.0f;
};