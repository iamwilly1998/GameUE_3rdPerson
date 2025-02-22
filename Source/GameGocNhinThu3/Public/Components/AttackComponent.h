// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enum/AttackType.h"
#include "AttackComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FHitSomeThingDelegate, const FHitResult&, HitResult);

class ACharacter;
class UBaseCharacterData;
class IAttackInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEGOCNHINTHU3_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackComponent();
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RequestAttack();
	bool CanAttack() const;
	void TraceHit();
	void SetupAttackComponent(UBaseCharacterData* BCD);
	void AN_EndAttack();
	void AN_ComboAttack();
	void SetupTraceHit();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void Attack();
	void HandleHitResult(const FHitResult& Result);
	UAnimMontage* GetCorrectAttackMontage();

public:
	FHitSomeThingDelegate HitSomeThingDelegate;
	EAttackType RequestAttackType = EAttackType::LightAttack;

	float StaminaAttackCost = 0.0f;
	float AttackDamage = 0.0f;

	bool bIsAttacking = false;
	int AttackCount = 0;

private:
	// Save Actor that have interface
	TScriptInterface<IAttackInterface> AttackInterface;
	UPROPERTY()
	UBaseCharacterData* BaseCharacterData;
	
	bool bCanCombo;
	bool bSavedAttack;
	int AttackIndex = 0;

	// Trace hit
	TArray<AActor*> HittedActors;
	//int HitCount = 0;
};
