// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStates/TraceHit_ANS.h"
#include "Interfaces/AttackInterface.h"

void UTraceHit_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp == nullptr)
		return;
	AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
	if(AttackInterface)
		AttackInterface->I_ANS_BeginTraceHit();
}

void UTraceHit_ANS::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// vung kiem
	// Trace hit
	// actor -> attack interface -> trace hit
	if(AttackInterface)
		AttackInterface->I_ANS_TraceHit();
}
