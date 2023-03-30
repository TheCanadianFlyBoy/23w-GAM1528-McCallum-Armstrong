// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCharacterUE5Character.h"
#include "FPSBombActor.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "BombDamageType.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "UnrealNetwork.h"

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
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	
		// Create a gun mesh component
	BombGripMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_BombGrip"));
	BombGripMeshComponent->CastShadow = false;
	BombGripMeshComponent->SetupAttachment(Mesh1P, "BombSocket");

	OnTakeAnyDamage.AddDynamic(this, &AFPSCharacterUE5Character::TakeAnyDamage);

	//SET HeldBomb to null
	HeldBomb = nullptr;

	IsAbleToFire = true;

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

void AFPSCharacterUE5Character::TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, "I, " + GetName() + ", have received " + FString::FromInt(Damage) + " Damage from the Damage Causer, hereby referred to as " + DamageCauser->GetName() + ", instigated by a third party, hereby referred to as " + InstigatedBy->GetName());

	const UBombDamageType* damageType = Cast<UBombDamageType>(DamageType);
	if (damageType)
	{
		GetCapsuleComponent()->SetSimulatePhysics(false);
		DisableInput(nullptr);
		SetHasRifle(false);
		IsAbleToFire = false;
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

		EnhancedInputComponent->BindAction(BombAction, ETriggerEvent::Triggered, this, &AFPSCharacterUE5Character::PickupBomb);
		EnhancedInputComponent->BindAction(BombAction, ETriggerEvent::Completed, this, &AFPSCharacterUE5Character::ThrowBomb);
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

void AFPSCharacterUE5Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFPSCharacterUE5Character::GetHasRifle()
{
	return bHasRifle;
}

void AFPSCharacterUE5Character::PickupBomb()
{
	if (HeldBomb)
		return;

	//Ray Cast to check if a Actor is hit by a raycast
	AActor* PickedActor = RayCastGetActor();
	//IF a Actor has been detected/hit
	if (PickedActor)
	{
		Server_PickupBomb(PickedActor);
	}
	//ENDIF
}

void AFPSCharacterUE5Character::Server_PickupBomb_Implementation(AActor* PickedActor)
{
	//Check if it is a AFPSBombActor by doing a cast
	AFPSBombActor* bomb = Cast<AFPSBombActor>(PickedActor);
	//IF it is a AFPSBombActor
	if (bomb)
	{
		//SET HeldBomb to the hit bomb
		HeldBomb = bomb;
		//CALL Hold on the bomb passing in GunMeshComponent, we will attach it to the Gun
		bomb->Hold(BombGripMeshComponent);
	}
	//ENDIF
	//
}

void AFPSCharacterUE5Character::ThrowBomb()
{
	Server_ThrowBomb(GetActorRotation().Vector());
}

void AFPSCharacterUE5Character::Server_ThrowBomb_Implementation(FVector Direction)
{
	//IF we have a HeldBomb
	if (HeldBomb)
	{
		//Throw the Held Bomb passing in the Camera's Forward Vector
		HeldBomb->Throw(Direction);
		HeldBomb = nullptr;
	}
	//ENDIF
}

AActor* AFPSCharacterUE5Character::RayCastGetActor()
{
	if (Controller && Controller->IsLocalPlayerController()) // we check the controller because we don't want bots to grab the use object and we need a controller for the GetPlayerViewpoint function
	{
		FVector StartTrace;
		FVector Direction;
		FVector EndTrace;

		SetupRay(StartTrace, Direction, EndTrace);

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult Hit(ForceInit);
		UWorld* World = GetWorld();
		//World->LineTraceSingleByObjectType(Hit, StartTrace, EndTrace, ObjectTypeQuery1, TraceParams); // simple trace function  ECC_PhysicsBody
		World->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_PhysicsBody, TraceParams); // simple trace function  ECC_PhysicsBody
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 1, 0, 1.f);
		return Hit.GetActor();
	}
	return nullptr;
}

void AFPSCharacterUE5Character::SetupRay(FVector& StartTrace, FVector& Direction, FVector& EndTrace)
{
	FVector CamLoc;
	FRotator CamRot;

	Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation

	StartTrace = CamLoc; // trace start is the camera location
	Direction = CamRot.Vector();
	EndTrace = StartTrace + Direction * 300;
}

void AFPSCharacterUE5Character::Tick(float DeltaTime)
{
	//SUPER
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		FRotator NewRot = FirstPersonCameraComponent->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.f / 255.f;

		FirstPersonCameraComponent->SetRelativeRotation(NewRot);
	}

}
