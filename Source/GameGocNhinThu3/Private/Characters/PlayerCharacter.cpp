// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Widgets/PlayerWidget.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "DataAssets/BaseCharacterData.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/EnhancedInputData.h"
#include "Components/AttackComponent.h"	

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

APlayerCharacter::APlayerCharacter()
{
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

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Player widget
	// World
	PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

	if (PlayerWidget && HealthComponent)
	{
		PlayerWidget->AddToViewport();
		PlayerWidget->UpdateHealthBar_Player(HealthComponent->Health, HealthComponent->MaxHealth);
		PlayerWidget->UpdateStaminaBar_Player(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
		PlayerWidget->HideEnemyStats();
	}
}

void APlayerCharacter::HandleTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	Super::HandleTakePointDamage(DamagedActor, Damage, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);
	if (PlayerWidget && HealthComponent)
		PlayerWidget->UpdateHealthBar_Player(HealthComponent->Health, HealthComponent->MaxHealth);
}

void APlayerCharacter::HandleDead()
{
	Super::HandleDead();
	if(PlayerWidget)
		PlayerWidget->RemoveFromParent();
	auto PlayerController = Cast<APlayerController>(GetController());
	DisableInput(PlayerController);
}

void APlayerCharacter::I_EnterCombat(float Health_Enemy, float MaxHealth_Enemy, float Stamina_Enemy, float MaxStamina_Enemy)
{
	if (PlayerWidget)
	{
		PlayerWidget->ShowEnemyStats();
		PlayerWidget->UpdateHealthBar_Enemy(Health_Enemy, MaxHealth_Enemy);
		PlayerWidget->UpdateStaminaBar_Enemy(Stamina_Enemy, MaxStamina_Enemy);
	}
	if (BaseCharacterData)
		ChangeWalkSpeed(BaseCharacterData->CombatSpeed);
}

void APlayerCharacter::I_HitTarget(float Health_Target, float MaxHealth_Target)
{
	if (PlayerWidget)
	{
		PlayerWidget->UpdateHealthBar_Enemy(Health_Target, MaxHealth_Target);
	}
}

void APlayerCharacter::I_HandleTargetDestroyed()
{
	if (PlayerWidget)
		PlayerWidget->HideEnemyStats();
}

void APlayerCharacter::I_HandleAttackSuccess()
{
	Super::I_HandleAttackSuccess();
	if (StaminaComponent)	
		PlayerWidget->UpdateStaminaBar_Player(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
}

void APlayerCharacter::I_HandleTargetAttacked(float Stamina_Target, float MaxStamina_Target)
{
	if (PlayerWidget)
		PlayerWidget->UpdateStaminaBar_Enemy(Stamina_Target, MaxStamina_Target);
}

#pragma region Input

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	CharacterAddMappingContext();
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Cast Player input component into enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// IA_Look -> Event Func -> Bind
	if (EnhancedInputData == nullptr)
		return;
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_LookAround, ETriggerEvent::Triggered, this, &APlayerCharacter::LookAround);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Moving, ETriggerEvent::Triggered, this, &APlayerCharacter::Moving);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Attack, ETriggerEvent::Started, this, &APlayerCharacter::AttackPressed);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_ExitCombat, ETriggerEvent::Started, this, &APlayerCharacter::ExitCombatPressed);

}

void APlayerCharacter::CharacterAddMappingContext()
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

void APlayerCharacter::LookAround(const FInputActionValue& Value)
{
	// Value get X,Y
	const FVector2D LookAroundValue = Value.Get<FVector2D>();
	if (LookAroundValue.X != 0.0)
		AddControllerYawInput(LookAroundValue.X);
	if (LookAroundValue.Y != 0.0)
		AddControllerPitchInput(LookAroundValue.Y);
}

void APlayerCharacter::Moving(const FInputActionValue& Value)
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

void APlayerCharacter::AttackPressed()
{
	I_RequestAttack();
}
void APlayerCharacter::ExitCombatPressed()
{
	// Hide enemy health
	// Player -> Exit combat -> noti to enemy ->  patrol
	if (PlayerWidget)
		PlayerWidget->HideEnemyStats();

	if (I_OnExitCombat.IsBound())
		I_OnExitCombat.Execute();
}
#pragma endregion


