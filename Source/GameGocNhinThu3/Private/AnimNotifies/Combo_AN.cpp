// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/Combo_AN.h"
#include "Interfaces/AttackInterface.h"

void UCombo_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Combo_AN"));
	if (MeshComp == NULL)
		return;
	auto AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
	if (AttackInterface)
		AttackInterface->I_AN_ComboAttack();
}
