// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCharacterUE5Character.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "BombDamageType.h"
#include "FPSBombActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"


//////////////////////////////////////////////////////////////////////////
// AFPSCharacterUE5Character

AFPSCharacterUE5Character::AFPSCharacterUE5Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	
		// Create a gun mesh component
	BombGripMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_BombGrip"));
	BombGripMeshComponent->CastShadow = false;
	BombGripMeshComponent->SetupAttachment(Mesh1P, "BombSocket");

	IsAbleToFire = true;

	OnTakeAnyDamage.AddDynamic(this, &AFPSCharacterUE5Character::OnDamage);
}

void AFPSCharacterUE5Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSCharacterUE5Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacterUE5Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacterUE5Character::Look);

		//Pickup
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &AFPSCharacterUE5Character::Pickup);
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Completed, this, &AFPSCharacterUE5Character::Throw);
	}
}


void AFPSCharacterUE5Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSCharacterUE5Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSCharacterUE5Character::Pickup(const FInputActionValue& Value)
{
		AActor* CastActor = RayCastGetActor();

		//Null guard
		if (CastActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Touched " + CastActor->GetName());
			AFPSBombActor* BombActor = Cast<AFPSBombActor>(CastActor);
			//If is a bomb
			if (BombActor)
			{
				HeldBomb = BombActor;
				BombActor->Hold(BombGripMeshComponent);
			}
		}

}

void AFPSCharacterUE5Character::Throw(const FInputActionValue& Value)
{
	if (HeldBomb)
	{
		HeldBomb->Throw(GetActorForwardVector());
	}
}

AActor* AFPSCharacterUE5Character::RayCastGetActor()
{
	if (Controller)
	{
		FVector StartTrace, Direction, EndTrace;
		SetupRay(StartTrace, Direction, EndTrace);
		
		//Setup params
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = false;

		FHitResult OutHit;
		GetWorld()->LineTraceSingleByChannel(OutHit, StartTrace, EndTrace, ECC_PhysicsBody, QueryParams);

		return OutHit.GetActor();
	}
	return nullptr;
}

void AFPSCharacterUE5Character::SetupRay(FVector& StartTrace, FVector& Direction, FVector& EndTrace)
{
	Direction = GetControlRotation().Vector();
	StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	EndTrace = StartTrace + Direction * 150.f;
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Blue, false, 5.f, 0, 2.f);
}

void AFPSCharacterUE5Character::OnDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageInstigator)
{
	int Msg_Index = FMath::Rand() % DamageMessage.Num();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "(" + FString::SanitizeFloat(DamageAmount) + ") ~ " + DamageMessage[Msg_Index]);

	DisableInput(GetLocalViewingPlayerController());

}

void AFPSCharacterUE5Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFPSCharacterUE5Character::GetHasRifle()
{
	return bHasRifle;
}
