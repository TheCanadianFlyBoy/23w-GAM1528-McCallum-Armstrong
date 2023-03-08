// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "ThirdPersonProjectile.h"

#include "TPGameStateBase.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "TPPlayerState.h"

//////////////////////////////////////////////////////////////////////////
// AThirdPersonCharacter

AThirdPersonCharacter::AThirdPersonCharacter()
{

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//Initialize projectile class
	ProjectileClass = AThirdPersonProjectile::StaticClass();
	//Initialize fire rate
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	//if(GetLocalRole() == ROLE_Authority)
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	DisplayText = CreateDefaultSubobject<UTextRenderComponent>("Text Renderer");
	
	DisplayText->SetupAttachment(RootComponent);

	//NETWORK
	bAlwaysRelevant = true;
	SetReplicates(true);

}
//NETWORK
void AThirdPersonCharacter::SetPlayTime()
{
	PlayTime += 1;
	OnRep_DisplayPlayTime();
}
//NETWORK
void AThirdPersonCharacter::OnRep_DisplayPlayTime()
{
	FString PlayerName = "";
	if(GetPlayerState())
		PlayerName = GetPlayerState<ATPPlayerState>()->PlayerName;

	int GameTime = 0;
	ATPGameStateBase* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<ATPGameStateBase>() : NULL;
	if (MyGameState)
		GameTime = MyGameState->GameTime;

	DisplayText->SetText("Local Role: " + NetRoleToText(GetLocalRole()) + "\nPlay Time: " + FString::FromInt(PlayTime) + "\nPlayerName = " + PlayerName + "\n GameTime = " + FString::FromInt(GameTime));
}

void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//REPLICATE  GameTime
	DOREPLIFETIME(AThirdPersonCharacter, PlayTime);
}

void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	//NETWORK
	DisplayText->SetText( "Local Role: " + NetRoleToText(GetLocalRole()));
	if (GetLocalRole() == ROLE_Authority) {
		FTimerHandle UpdateHandle;
		GetWorldTimerManager().SetTimer(UpdateHandle, this, &AThirdPersonCharacter::SetPlayTime, 1.0f, true, 0.0f);
	}

}
//NETWORK
void AThirdPersonCharacter::StartFire()
{
	Server_StartFire();
}
void AThirdPersonCharacter::Server_StartFire_Implementation()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AThirdPersonCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}
//NETWORK
void AThirdPersonCharacter::StopFire()
{
	Server_StopFire();
}

void AThirdPersonCharacter::Server_StopFire_Implementation()
{
	bIsFiringWeapon = false;
}

//NETWORK
void AThirdPersonCharacter::HandleFire()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
	
	AThirdPersonProjectile* spawnedProjectile = GetWorld()->SpawnActor<AThirdPersonProjectile>(spawnLocation, spawnRotation, spawnParameters);
	
}
//NETWORK
void AThirdPersonCharacter::SprintOn()
{
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	Server_SprintOn();
}

void AThirdPersonCharacter::Server_SprintOn_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
}

//NETWORK
void AThirdPersonCharacter::SprintOff()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	Server_SprintOff();
}

void AThirdPersonCharacter::Server_SprintOff_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

FString AThirdPersonCharacter::NetRoleToText(ENetRole role)
{
	FString roleStr;
	switch (role)
	{
	case ENetRole::ROLE_None:
		roleStr = "None";
		break;

	case ENetRole::ROLE_SimulatedProxy:
		roleStr = "SimulatedProxy";
		break;

	case ENetRole::ROLE_AutonomousProxy:
		roleStr = "AutonomousProxy";
		break;

	case ENetRole::ROLE_Authority:
		roleStr = "Authority";
		break;
	}

	return roleStr;
}

//NETWORK

/*
 UENUM(BlueprintType)
enum ELifetimeCondition
{
	COND_None = 0							UMETA(DisplayName = "None"),							// This property has no condition, and will send anytime it changes
	COND_InitialOnly = 1					UMETA(DisplayName = "Initial Only"),					// This property will only attempt to send on the initial bunch
	COND_OwnerOnly = 2						UMETA(DisplayName = "Owner Only"),						// This property will only send to the actor's owner
	COND_SkipOwner = 3						UMETA(DisplayName = "Skip Owner"),						// This property send to every connection EXCEPT the owner
	COND_SimulatedOnly = 4					UMETA(DisplayName = "Simulated Only"),					// This property will only send to simulated actors
	COND_AutonomousOnly = 5					UMETA(DisplayName = "Autonomous Only"),					// This property will only send to autonomous actors
	COND_SimulatedOrPhysics = 6				UMETA(DisplayName = "Simulated Or Physics"),			// This property will send to simulated OR bRepPhysics actors
	COND_InitialOrOwner = 7					UMETA(DisplayName = "Initial Or Owner"),				// This property will send on the initial packet, or to the actors owner
	COND_Custom = 8							UMETA(DisplayName = "Custom"),							// This property has no particular condition, but wants the ability to toggle on/off via SetCustomIsActiveOverride
	COND_ReplayOrOwner = 9					UMETA(DisplayName = "Replay Or Owner"),					// This property will only send to the replay connection, or to the actors owner
	COND_ReplayOnly = 10					UMETA(DisplayName = "Replay Only"),						// This property will only send to the replay connection
	COND_SimulatedOnlyNoReplay = 11			UMETA(DisplayName = "Simulated Only No Replay"),		// This property will send to actors only, but not to replay connections
	COND_SimulatedOrPhysicsNoReplay = 12	UMETA(DisplayName = "Simulated Or Physics No Replay"),	// This property will send to simulated Or bRepPhysics actors, but not to replay connections
	COND_SkipReplay = 13					UMETA(DisplayName = "Skip Replay"),						// This property will not send to the replay connection
	COND_Never = 15							UMETA(Hidden),											// This property will never be replicated
	COND_Max = 16							UMETA(Hidden)
};
 */

 //////////////////////////////////////////////////////////////////////////
 // Input

void AThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AThirdPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AThirdPersonCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonCharacter::SprintOn);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonCharacter::SprintOff);

	// Handle firing projectiles
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AThirdPersonCharacter::StartFire);

}

void AThirdPersonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AThirdPersonCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}