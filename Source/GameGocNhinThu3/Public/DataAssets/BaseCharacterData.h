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
	// T<Array> to have more than one attack montage
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TArray<UAnimMontage*> AttackMontages;

	// Hit React Monatge

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Front;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Back;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Left;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Right;
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
