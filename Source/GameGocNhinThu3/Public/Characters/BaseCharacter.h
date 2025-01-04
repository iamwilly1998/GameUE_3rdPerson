#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/AttackInterface.h"
#include "Enum/CombatState.h"

#include "BaseCharacter.generated.h"


class UAttackComponent;
class UHealthComponent;
class UStaminaComponent;
class UEnhancedInputData;
class UBaseCharacterData;


UCLASS()
class GAMEGOCNHINTHU3_API ABaseCharacter : public ACharacter, public IAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void PostInitializeComponents() override;
	//virtual void Tick(float DeltaSeconds) override;
	
	void ChangeWalkSpeed(float WalkSpeed);

	// Attack Interface
#pragma region Attack Interface
	virtual void I_PlayAttackMontage(UAnimMontage* AttackMontage) override;
	virtual void I_PlayAttackingSound() override;

	virtual void I_AN_EndAttack() override;
	virtual void I_AN_EndHitReact() override;
	virtual void I_AN_ComboAttack() override;
	virtual FVector I_GetSocketLocation(const FName& SocketName) const override;
	virtual void I_ANS_TraceHit() override;
	virtual void I_ANS_BeginTraceHit() override;
	virtual void I_RequestAttack() override;
	virtual void I_HandleAttackSuccess() override;
	virtual bool I_DoesReadyToAttack() const override;
	virtual bool I_HasEnoughStamina(float Cost) const override;

#pragma endregion
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleTakePointDamage(
		AActor* DamagedActor,
		float Damage,
		class AController* InstigatedBy,
		FVector HitLocation,
		class UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const class UDamageType* DamageType,
		AActor* DamageCauser);

	virtual void HandleDead();




private:
	// Attack Direction
	UAnimMontage* GetCorrectHitReactMontage(const FVector& AttackDirection) const;

	//Event Function
	UFUNCTION()
	void HandleHitSomething(const FHitResult& HitResult);

	void SpawnHitImpact(const FVector& HitLocation);
	void HandleAttacked(const FVector& ShotFromDirection);

	
protected:

	UPROPERTY(VisibleAnywhere)
	UAttackComponent* AttackComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere)
	UStaminaComponent* StaminaComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Character Data Assets")
	UBaseCharacterData* BaseCharacterData;


	UPROPERTY(EditDefaultsOnly, Category = "Character Data Assets")
	UEnhancedInputData* EnhancedInputData;

private:

	ECombatState CombatState = ECombatState::Ready;

	


	// Getter vs Setter
public:
	FORCEINLINE
	ECombatState GetCombatState() const { return CombatState; }

};
