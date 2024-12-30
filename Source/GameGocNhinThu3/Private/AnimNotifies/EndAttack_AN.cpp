// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/EndAttack_AN.h"
#include "Interfaces/AttackInterface.h"

void UEndAttack_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	//// Print notification
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	1.0f,
	//	FColor::Cyan,
	//	TEXT("End Attack Notify")
	//);
	// Owner -> Attack Interface -> End Attack
	if (MeshComp == nullptr)
		return;
	auto AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
	if (AttackInterface)
		AttackInterface->I_AN_EndAttack();
}
