// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

class UBaseCharacterData;
class IAttackInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEGOCNHINTHU3_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupComponent(UBaseCharacterData* BCD);
	void UpdateStamina(float Cost);

protected:
	virtual void BeginPlay() override;
	
public:
	float Stamina = 0.0f;
	float MaxStamina = 0.0f;
	float RegenStaminaRate = 0.0f;

private:
	UPROPERTY();
	TScriptInterface<IAttackInterface> AttackInterface;
};
