#include "Characters/BaseCharacter.h"
#include "DataAssets/BaseCharacterData.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AttackComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"



// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Actor component
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("Attack Component"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));


	// Prevent character move with controller
	bUseControllerRotationYaw = false;
	// Character movement component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Rotation rate Yaw = Z
	GetCharacterMovement()->RotationRate.Yaw = 540.0;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}



void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Attack component 
	if (AttackComponent)
	{
		// dang ky delegate
		AttackComponent->HitSomeThingDelegate.BindDynamic(this, &ABaseCharacter::HandleHitSomething);
		AttackComponent->SetupAttackComponent(BaseCharacterData);
	}
	// Health component
	if (HealthComponent)
		HealthComponent->SetupComponent(BaseCharacterData);

	// Stamina component
	if (StaminaComponent)
		StaminaComponent->SetupComponent(BaseCharacterData);
}

void ABaseCharacter::ChangeWalkSpeed(float WalkSpeed)
{
	if(GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	// Combat Speed
	// Default Speed
	// Patrol Speed
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &ABaseCharacter::HandleTakePointDamage);
	if (BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->DefaultSpeed);
}

#pragma region AttackInterface

void ABaseCharacter::I_EnterCombat(AActor* TargetActor)
{
	AttackInterface_Target = TScriptInterface<IAttackInterface>(TargetActor);

	Strafe();
}

void ABaseCharacter::Strafe()
{
	if (BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->CombatSpeed);

	bUseControllerRotationYaw = true;
	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = false;
	bIsStrafing = true;
}

void ABaseCharacter::NotStrafe()
{
	if (BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->DefaultSpeed);

	bUseControllerRotationYaw = false;
	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsStrafing = false;
}

void ABaseCharacter::I_HandleExitCombat()
{
	NotStrafe();
}

void ABaseCharacter::I_ExitCombat()
{
	NotStrafe();
}

void ABaseCharacter::I_PlayAttackMontage(UAnimMontage* AttackMontage)
{
	PlayAnimMontage(AttackMontage);
}

void ABaseCharacter::I_PlayAttackingSound()
{
	if (BaseCharacterData)
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			BaseCharacterData->AttackingSound, 
			GetActorLocation());
}

void ABaseCharacter::I_AN_EndAttack()
{
	// Attack Component
	// bIsAttacking -> false
	if(AttackComponent)
		AttackComponent->AN_EndAttack();
}

void ABaseCharacter::I_AN_EndHitReact()
{
	CombatState = ECombatState::Ready;
	I_AN_EndAttack();
}

void ABaseCharacter::I_AN_ComboAttack()
{
	if (AttackComponent)
		AttackComponent->AN_ComboAttack();
}

FVector ABaseCharacter::I_GetSocketLocation(const FName& SocketName) const
{
	if (GetMesh() == nullptr)
		return FVector();
	return GetMesh()->GetSocketLocation(SocketName);
}

void ABaseCharacter::I_ANS_BeginTraceHit()
{
	if (AttackComponent)
		AttackComponent->SetupTraceHit();
}

void ABaseCharacter::I_RequestAttack()
{

	if (AttackComponent)
		AttackComponent->RequestAttack();
}

void ABaseCharacter::I_HandleAttackSuccess()
{
	if(StaminaComponent && AttackComponent)
		StaminaComponent->UpdateStamina(AttackComponent->StaminaAttackCost);
}

bool ABaseCharacter::I_DoesReadyToAttack() const
{
	return CombatState == ECombatState::Ready;
}

bool ABaseCharacter::I_HasEnoughStamina(float Cost) const
{
	if (StaminaComponent == nullptr) return false;
	return StaminaComponent->Stamina >= Cost;
}

bool ABaseCharacter::I_IsInCombat() const
{
	if (AttackComponent == nullptr)
		return false;
	return AttackComponent->bIsAttacking;
}

float ABaseCharacter::I_GetHealth() const
{
	return HealthComponent ? HealthComponent->Health : 0.0f;
}

float ABaseCharacter::I_GetMaxHealth() const
{
	return HealthComponent ? HealthComponent->MaxHealth : 0.0f;
}

float ABaseCharacter::I_GetStamina() const
{
	return StaminaComponent ? StaminaComponent->Stamina : 0.0f;
}

float ABaseCharacter::I_GetMaxStamina() const
{
	return StaminaComponent ? StaminaComponent->MaxStamina : 0.0f;
}

void ABaseCharacter::I_ANS_TraceHit()
{
	if (AttackComponent)
		AttackComponent->TraceHit();
}

#pragma endregion



void ABaseCharacter::HandleHitSomething(const FHitResult& HitResult)
{
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		1.0f,
		FColor::Cyan,
		TEXT("Handle Hit Something")
	);*/

	// Game Play Kismet
	// Apply point damage
	// Hit from direction
	// vi tri ng tan cong - vi tri ng bi tan cong
	if (BaseCharacterData == nullptr)
		return;
	if (AttackComponent == nullptr)
		return;

	auto HitActor = HitResult.GetActor();
	auto AttackDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitActor->GetActorLocation());

	UGameplayStatics::ApplyPointDamage(
		HitActor,
		AttackComponent->AttackDamage,
		AttackDirection,
		HitResult,
		GetController(),
		// vu khi gan vao nhan vat nen xai this
		this,
		UDamageType::StaticClass()
		);
}

void ABaseCharacter::HandleTakePointDamage(
	AActor* DamagedActor, 
	float Damage, 
	AController* InstigatedBy, 
	FVector HitLocation, 
	UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const UDamageType* DamageType, 
	AActor* DamageCauser)
{
	SpawnHitImpact(HitLocation);
	// Update Health
	if (HealthComponent)
		HealthComponent->UpdateHealthByDamage(Damage);

	
	
	// mau > 0 -> attacked
	// mau <= 0 -> death
	if (HealthComponent->Health > 0.0f)
		HandleAttacked(ShotFromDirection);
	else
		HandleDead();
}

void ABaseCharacter::HandleDead()
{
	// Death Sound
	// Death Animation
	if (BaseCharacterData == nullptr)
		return;
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BaseCharacterData->DeadSound,
		GetActorLocation());

	float DeadMontageSecond = PlayAnimMontage(BaseCharacterData->DeadMontage);
	CombatState = ECombatState::Dead;

	if (GetCharacterMovement())
		GetCharacterMovement()->StopMovementImmediately();

	if (GetCapsuleComponent() && GetMesh()) 
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetLifeSpan(DeadMontageSecond);
}

void ABaseCharacter::HandleAttacked(const FVector& ShotFromDirection)
{
	if (BaseCharacterData == nullptr)
		return;
	// Play Pain Sound, Pain from character -> Actor Location
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BaseCharacterData->PainSound,
		GetActorLocation());

	// hit react animation
	PlayAnimMontage(GetCorrectHitReactMontage(ShotFromDirection));
	CombatState = ECombatState::Attacked;

}

void ABaseCharacter::SpawnHitImpact(const FVector& HitLocation)
{
	if (BaseCharacterData == nullptr)
		return;
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		BaseCharacterData->HitImpactEffect,
		HitLocation);

	// Play Hit Impact Sound
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BaseCharacterData->HitImpactSound,
		HitLocation);
}

UAnimMontage* ABaseCharacter::GetCorrectHitReactMontage(const FVector& AttackDirection) const
{
	if (BaseCharacterData == nullptr)
		return nullptr;
	// front
	// back
	// Use Dot Product to determine front or back
	// 1 = back , -1 = front
	const double Dot = FVector::DotProduct(AttackDirection, GetActorForwardVector());
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		1.0f,
		FColor::Red,
		FString::Printf(TEXT("DOT = %f"), Dot)
	);*/
	bool bShouldUseDot = FMath::Abs(Dot) > 0.5;
	if (bShouldUseDot)
	{
		if (Dot > 0.0)
			return BaseCharacterData->HitReactMontage_Back;
		return BaseCharacterData->HitReactMontage_Front;
	}
	else
	{
		const FVector Cross = FVector::CrossProduct(AttackDirection, GetActorForwardVector());
		if (Cross.Z > 0.0)
			return BaseCharacterData->HitReactMontage_Right;
		return BaseCharacterData->HitReactMontage_Left;
	}
	return nullptr;
}