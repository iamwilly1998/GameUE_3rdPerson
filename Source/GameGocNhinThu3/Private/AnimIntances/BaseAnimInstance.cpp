// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimIntances/BaseAnimInstance.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enum/CombatState.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	//Pawn -> Character 
	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (Character)
		MovementComponent = Character->GetCharacterMovement();
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// Speed <- Movement Component <- Character
	// Use kismet to calculate speed from velocity ( van toc)
	if (MovementComponent == nullptr && Character == nullptr)
		return;

	GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	bShouldMove = GroundSpeed > 0.0f;
	// When BlendLowerUpper ?
	// Moving && haven't play react animation
	// BaseCharacter add state and check if it hit
	// CombatState
	// Enum
	// Ready , Attacked
	Character->GetCombatState();
	bShouldBlendLowerUpper = GroundSpeed > 0.0f && Character->GetCombatState() == ECombatState::Ready;	
}
