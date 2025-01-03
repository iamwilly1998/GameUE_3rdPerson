// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "BaseCharacterData.generated.h"

class USoundBase;
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

	// Hit React Montage

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Front;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Back;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Left;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	UAnimMontage* HitReactMontage_Right;
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	USoundBase* PainSound;

	// Dead Montage
	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	UAnimMontage* DeadMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	USoundBase* DeadSound;

	// Hit Impact
	UPROPERTY(EditDefaultsOnly, Category = "Hit Impact")
	UParticleSystem* HitImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* HitImpactSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* AttackingSound;


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
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	float TraceRadius = 40.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Hit")
	float DrawTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Damage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Health = 100.0f; 
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Stamina = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxStamina = 100.0f;

	// Speed
	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float DefaultSpeed = 600.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float CombatSpeed = 400.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float PatrolSpeed = 300.0f;
	
};
