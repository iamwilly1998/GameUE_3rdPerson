// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Widgets/PlayerWidget.h"
#include "Widgets/EndWidget.h"

#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/AttackComponent.h"	
#include "Components/AudioComponent.h"

#include "DataAssets/BaseCharacterData.h"
#include "DataAssets/EnhancedInputData.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

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

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	CharacterAddMappingContext();

	// Cast Player input component into enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// IA_Look -> Event Func -> Bind
	if (EnhancedInputData == nullptr)
		return;

	EnhancedInputComponent->BindAction(EnhancedInputData->IA_LookAround, ETriggerEvent::Triggered, this, &APlayerCharacter::LookAround);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Moving, ETriggerEvent::Triggered, this, &APlayerCharacter::Moving);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_Attack, ETriggerEvent::Started, this, &APlayerCharacter::AttackPressed);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_StrongAttack, ETriggerEvent::Started, this, &APlayerCharacter::StrongAttackPressed);
	EnhancedInputComponent->BindAction(EnhancedInputData->IA_ExitCombat, ETriggerEvent::Started, this, &APlayerCharacter::ExitCombatPressed);

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

	if (PlayerWidget && HealthComponent)
	{
		PlayerWidget->AddToViewport();
		PlayerWidget->UpdateHealthBar_Player
		(HealthComponent->Health, HealthComponent->MaxHealth);

		PlayerWidget->UpdateStaminaBar_Player
		(StaminaComponent->Stamina, StaminaComponent->MaxStamina);

		PlayerWidget->HideEnemyStats();

		PlayerWidget->UpdateKills(Kills);

	}
	// game play statics
	if (BaseCharacterData == nullptr) return;
	BackgroundAudio = UGameplayStatics::SpawnSound2D(this, BaseCharacterData->BackgroundThemeSound);

	if (BackgroundAudio)
		BackgroundAudio->SetVolumeMultiplier(BaseCharacterData->BackgroundAudioVolume);
}

void APlayerCharacter::Destroyed()
{
	if (AttackInterface_Target)
		AttackInterface_Target->I_HandleTargetDestroyed();

	Super::Destroyed();
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

	ShowEndWidget(LoseText);
}

void APlayerCharacter::HandleAttacked(const FVector& ShotFromDirection)
{
	Super::HandleAttacked(ShotFromDirection);
	// shake camera
	// camera manager
	auto CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (CameraManager && BaseCharacterData)
	{
		CameraManager->StartCameraShake(
			BaseCharacterData->CameraShakeClass,
			BaseCharacterData->ShakeScale
		);
	}
}

void APlayerCharacter::I_EnterCombat(AActor* TargetActor)
{
	Super::I_EnterCombat(TargetActor);

	ShowTargetStats();

	PlayThemeSound_Combat();
}

void APlayerCharacter::I_ReceiveCombat(AActor* TargetActor)
{
	Super::I_ReceiveCombat(TargetActor);

	ShowTargetStats();

	PlayThemeSound_Combat();
}

void APlayerCharacter::I_ExitCombat()
{
	Super::I_ExitCombat();

	// Hide enemy health
	// Player -> Exit combat -> noti to enemy ->  patrol
	if (PlayerWidget)
		PlayerWidget->HideEnemyStats();

	if (AttackInterface_Target)
		AttackInterface_Target->I_HandleExitCombat();

	PlayThemeSound_Background();
}

void APlayerCharacter::ShowTargetStats()
{
	if (AttackInterface_Target == nullptr) return;
	
	if (PlayerWidget)
	{
		PlayerWidget->ShowEnemyStats();

		PlayerWidget->UpdateHealthBar_Enemy(AttackInterface_Target->I_GetHealth(), AttackInterface_Target->I_GetMaxHealth());

		PlayerWidget->UpdateStaminaBar_Enemy(AttackInterface_Target->I_GetStamina(), AttackInterface_Target->I_GetMaxStamina());
	}
}

void APlayerCharacter::ShowEndWidget(FText ResultText)
{
	auto PlayerController = Cast<APlayerController>(GetController());

	if (EndWidget == nullptr)
		EndWidget = CreateWidget<UEndWidget>(PlayerController, EndWidgetClass);

	if (PlayerController == nullptr) return;
	if (EndWidget == nullptr) return;

	// Pause game
	// UGameplayStatics::SetGamePaused(this, true);

	EndWidget->AddToViewport();
	EndWidget->UpdateResultText(ResultText);

	FInputModeUIOnly MyInputMode;
	MyInputMode.SetWidgetToFocus(EndWidget->TakeWidget());

	PlayerController->SetInputMode(MyInputMode);
	PlayerController->SetShowMouseCursor(true);
}

void APlayerCharacter::PlayThemeSound_Background()
{
	if (BackgroundAudio && BaseCharacterData)
		BackgroundAudio->SetSound(BaseCharacterData->BackgroundThemeSound);
}

void APlayerCharacter::PlayThemeSound_Combat()
{
	if (BackgroundAudio && BaseCharacterData)
		BackgroundAudio->SetSound(BaseCharacterData->CombatThemeSound);
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
	Kills++;

	if (PlayerWidget)
	{
		PlayerWidget->HideEnemyStats();
		PlayerWidget->UpdateKills(Kills);
	}

	PlayThemeSound_Background();
	NotStrafe();

	if (Kills >= 1)
		ShowEndWidget(WinText);
}

void APlayerCharacter::I_HandleAttackSuccess()
{
	Super::I_HandleAttackSuccess();

	if (PlayerWidget && StaminaComponent)
		PlayerWidget->UpdateStaminaBar_Player(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
}

void APlayerCharacter::I_HandleTargetAttacked(float Stamina_Target, float MaxStamina_Target)
{
	if (PlayerWidget)
		PlayerWidget->UpdateStaminaBar_Enemy(Stamina_Target, MaxStamina_Target);
}

void APlayerCharacter::I_RegenStamina()
{
	if (PlayerWidget && StaminaComponent)
		PlayerWidget->UpdateStaminaBar_Player(StaminaComponent->Stamina, StaminaComponent->MaxStamina);
}

void APlayerCharacter::I_Target_RegenStamina(float Stamina_Target, float MaxStamina_Target)
{
	if (PlayerWidget)
		PlayerWidget->UpdateStaminaBar_Enemy(Stamina_Target, MaxStamina_Target);
}

#pragma region Input

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
	// Light Attack
	AttackComponent->RequestAttackType = EAttackType::LightAttack;
	I_RequestAttack();
}
void APlayerCharacter::StrongAttackPressed()
{
	AttackComponent->RequestAttackType = EAttackType::StrongAttack;
	I_RequestAttack();
}
void APlayerCharacter::ExitCombatPressed()
{
	I_ExitCombat();
}
#pragma endregion


