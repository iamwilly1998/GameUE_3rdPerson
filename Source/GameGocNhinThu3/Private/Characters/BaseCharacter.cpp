#include "Characters/BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "DataAssets/EnhancedInputData.h"
#include "DataAssets/BaseCharacterData.h"
#include "Components/AttackComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Spring arm 
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->AddLocalOffset(FVector(0.0f, 0.0f, 40.0f));
	// Follow Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	// Actor component
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("Attack Component"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	// Prevent character move with controller
	bUseControllerRotationYaw = false;

	// Character movement component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Rotation rate Yaw = Z
	GetCharacterMovement()->RotationRate.Yaw = 540.0;

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	CharacterAddMappingContext();
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Cast Player input component into enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// IA_Look -> Event Func -> Bind
	if (EnhancedInputData == nullptr)
		return;
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_LookAround, ETriggerEvent::Triggered, this, &ABaseCharacter::LookAround);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Moving, ETriggerEvent::Triggered, this, &ABaseCharacter::Moving);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Attack, ETriggerEvent::Started, this, &ABaseCharacter::AttackPressed);
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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &ABaseCharacter::HandleTakePointDamage);
}

#pragma region AttackInterface

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

void ABaseCharacter::I_ANS_TraceHit()
{
	if (AttackComponent)
		AttackComponent->TraceHit();
}

#pragma endregion

void ABaseCharacter::CharacterAddMappingContext()
{
	// Get controller cast to Player controller
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
		return;
	// get Local Player then get subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// Add mapping context
	if (Subsystem && EnhancedInputData)
		Subsystem->AddMappingContext(EnhancedInputData->InputMappingContext, 0);
}


void ABaseCharacter::LookAround(const FInputActionValue& Value)
{
	 // Value get X,Y
	const FVector2D LookAroundValue = Value.Get<FVector2D>();
	if(LookAroundValue.X != 0.0)
		AddControllerYawInput(LookAroundValue.X);
	if (LookAroundValue.Y != 0.0)
		AddControllerPitchInput(LookAroundValue.Y);
}

void ABaseCharacter::Moving(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();
	// FRotator( Pitch = Y, Yaw = Z, Roll = X)
	const FRotator MyControllerRotation = FRotator(0.0, GetControlRotation().Yaw, 0.0);
	// Move forward, backward
	FVector ForwardDirection = MyControllerRotation.RotateVector(FVector::ForwardVector);
	if (MoveValue.Y != 0.0)
		AddMovementInput(ForwardDirection, MoveValue.Y);
	// Left right
	FVector RightDirection = MyControllerRotation.RotateVector(FVector::RightVector);
	if (MoveValue.X != 0.0)
		AddMovementInput(RightDirection, MoveValue.X);
}

void ABaseCharacter::AttackPressed()
{
	if(AttackComponent)
		AttackComponent->RequestAttack();

}

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

	auto HitActor = HitResult.GetActor();
	auto AttackDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitActor->GetActorLocation());

	UGameplayStatics::ApplyPointDamage(
		HitActor,
		BaseCharacterData->Damage,
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
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		1.0f,
		FColor::Cyan,
		TEXT("Handle Take Point")
	);*/
	
	// Hit Impact effect
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

	// Play Pain Sound, Pain from character -> Actor Location
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BaseCharacterData->PainSound,
		HitLocation);

	// hit react animation
	// Get correct react montage
	PlayAnimMontage(GetCorrectHitReactMontage(ShotFromDirection));
	//PlayAnimMontage(BaseCharacterData->HitReactMontage);
	CombatState = ECombatState::Attacked;
		
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