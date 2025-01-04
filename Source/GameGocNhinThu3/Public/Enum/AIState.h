#pragma once

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrol,
	Combat,
	Regen,
	Attack
};

// Patro 0, Combat 1, Attack 2