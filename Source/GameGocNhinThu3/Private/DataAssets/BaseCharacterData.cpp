// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/BaseCharacterData.h"

UBaseCharacterData::UBaseCharacterData()
{
	CostMap.Emplace(EAttackType::LightAttack, 20.0f);
	CostMap.Emplace(EAttackType::StrongAttack, 40.0f);

	DamageMap.Emplace(EAttackType::LightAttack, 20.0f);
	DamageMap.Emplace(EAttackType::StrongAttack, 40.0f);
}
