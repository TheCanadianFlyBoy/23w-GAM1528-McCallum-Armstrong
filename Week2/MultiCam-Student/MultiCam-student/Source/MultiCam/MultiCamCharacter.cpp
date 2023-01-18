// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MultiCamCharacter.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// AMultiCamCharacter

AMultiCamCharacter::AMultiCamCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	SetRootComponent(GetCapsuleComponent());

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	//SET bUseControllerRotationPitch to false
	bUseControllerRotationPitch = false;
	//SET bUseControllerRotationYaw to false
	bUseControllerRotationYaw = false;
	//SET bUseControllerRotationRoll to false
	bUseControllerRotationRoll = false;

	// Configure character movement

	//GET the Character Movement component
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	//SET the gravity scale on the movement comp to 1.5
	CharacterMovement->GravityScale = 1.5f;
	//SET Jump Z Velocity to 620
	CharacterMovement->JumpZVelocity = 620.f;
	//SET MaxWalkSpeedCrouched to 200
	CharacterMovement->MaxWalkSpeedCrouched = 200.f;
	//SET bOrientRotationToMovement to true
	CharacterMovement->bOrientRotationToMovement = true;          // Character moves in the direction of input...
	//SET RotateRate on the movement component to FRotator(0.0f, 540.0f, 0.0f)
	CharacterMovement->RotationRate = FRotator(0.f, 540.f, 0.f);  // ...at this rotation rate
	//SET the aircontrol on the movement component to 0.2f
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CREATE the CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	//ATTACH the CameraBoom to the CapsuleComponent
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	//SET CameraBoom->bUsePawnControlRotation to true
	CameraBoom->bUsePawnControlRotation = true;    // Rotate the arm based on the controller

	// Create a follow camera
	//CREATE the FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	//ATTAch The FollowCamera to the CameraBoom at USpringArmComponent::SocketName
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//SET bUsePawnControlRotation on the FollowCamera to false
	FollowCamera->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm
	
	// First person camera and arms
	//
	//CREATE the First Person Camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	//ATTACH the FirstPersonCamera to the Capsule Component
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	//CALL SetRelativeLocation() on the FirstPersonCamera passing in FVector(-39.56f, 1.75f, 64.f)
	FirstPersonCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));		// Position the camera
	//SET FirstPersonCamera->bUsePawnControlRotation = true to true
	FirstPersonCamera->bUsePawnControlRotation = true;

	//CREATE the Arms Mesh
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms Mesh"));
	//CALL SetOnlyOwnerSee() on the ArmsMesh to true
	ArmsMesh->SetOnlyOwnerSee(true);
	//ATTACH the ArmsMesh to the FirstPersonCamera
	ArmsMesh->SetupAttachment(FirstPersonCamera);
	//SET ArmsMesh->bCastDynamicShadow to false
	ArmsMesh->bCastDynamicShadow = false;
	//SET ArmsMesh->CastShadow to false
	ArmsMesh->CastShadow = false;
	//CALL ArmsMesh->SetRelativeRotation() on the ArmsMesh passing in FRotator(1.9f, -19.19f, 5.2f)
	ArmsMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	//CALL SetRelativeLocation() on the ArmsMesh passing in FVector(-0.5f, -4.4f, -155.7f)
	ArmsMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//CREATE the BodyMesh
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body Mesh"));
	//CALL SetOnlyOwnerSee() on the BodyMesh passing in true
	BodyMesh->SetOnlyOwnerSee(true);
	//ATTACH the BodyMesh to the Capsule Component
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	//SET BodyMesh->bCastDynamicShadow to false
	BodyMesh->bCastDynamicShadow = false;
	//SET BodyMesh->CastShadow to false
	BodyMesh->CastShadow = false;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	//CREATE the OverShoulderCameraBoom
	OverShoulderCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Over Shoulder Camera Boom"));
	//ATTACH the OverShoulderCameraBoom to the Capsule Component
	OverShoulderCameraBoom->SetupAttachment(GetCapsuleComponent());
	//SET OverShoulderCameraBoom->bUsePawnControlRotation to false
	OverShoulderCameraBoom->bUsePawnControlRotation = false;		// Fixed Camera
	//SET OverShoulderCameraBoom->TargetArmLength to 75.0f
	OverShoulderCameraBoom->TargetArmLength = 75.f;

	
	// Create a follow camera
	//CREATE the OverShoulderCamera
	OverShoulderCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Over Shoulder Camera"));
	//ATTACH the OverShoulderCamera to the OverShoulderCameraBoom at USpringArmComponent::SocketName
	OverShoulderCamera->SetupAttachment(OverShoulderCameraBoom, USpringArmComponent::SocketName);		// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//SET OverShoulderCamera->bUsePawnControlRotation to false
	OverShoulderCamera->bUsePawnControlRotation = false;

	//GET the Mesh from ACharacter and assign it to a variable called pMesh (This is the main mesh)
	USkeletalMeshComponent* pMesh = GetMesh();
	//SET pMesh->bCastDynamicShadow to true
	pMesh->bCastDynamicShadow = true;
	//SET pMesh->CastShadow to true
	pMesh->CastShadow = true;
	//SET pMesh->bCastHiddenShadow to true
	pMesh->bCastHiddenShadow = true;

	//SET CameraFollowTurnAngleExponent to .25f
	CameraFollowTurnRate = 0.25f;
	//SET CameraFollowTurnRate to .3f
	CameraFollowTurnRate = 0.3f;
	//SET CameraResetSpeed to 1.f
	CameraResetSpeed = 1.f;

	//SET CameraModeEnum to ECharacterCameraMode::ThirdPersonDefault
	CameraModeEnum = ECharacterCameraMode::ThirdPersonDefault;

	//SET bIsResetting to false
	bIsResetting = false;

	//SET CameraZoomMaximumDistance to 600.f
	CameraZoomMaximumDistance = 600.f;
	//SET CameraZoomMinimumDistance to 100.f
	CameraZoomMinimumDistance = 100.f;
	//SET CameraZoomCurrent to 300.f
	CameraZoomCurrent = 300.f;
	//SET CameraZoomIncrement to 20.f
	CameraZoomIncrement = 20.f;
	//SET CameraBoom->TargetArmLength to CameraZoomCurrent
	CameraBoom->TargetArmLength = CameraZoomCurrent;

	//SET AutoResetSmoothFollowCameraWhenIdle to true
	AutoResetSmoothFollowCameraWhenIdle = true;
	//SET AutoResetDelaySeconds to 2.5f
	AutoResetDelaySeconds = 2.5f;

	//SET IsAutoReset to false
	IsAutoReset = false;
	//SET AutoResetSpeed to .15f
	AutoResetSpeed = .15f;
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiCamCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiCamCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiCamCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AMultiCamCharacter::HandleYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiCamCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiCamCharacter::LookUpAtRate);

	// Camera
	InputComponent->BindAction("ToggleCameraMode", IE_Pressed, this, &AMultiCamCharacter::CycleCamera);
	PlayerInputComponent->BindAction("CameraZoomIn", IE_Pressed, this, &AMultiCamCharacter::ZoomCameraIn);
	PlayerInputComponent->BindAction("CameraZoomOut", IE_Pressed, this, &AMultiCamCharacter::ZoomCameraOut);
}

void AMultiCamCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//CALL 
	UpdateForCameraMode();
	
}

void AMultiCamCharacter::ZoomCameraIn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Zoom Camera In Requested");
	//DECREMENT CameraZoomCurrent by CameraZoomIncrement
	CameraZoomCurrent -= CameraZoomIncrement;
	//IF CameraZoomCurrent LESS THAN CameraZoomMinimumDistance
	if (CameraZoomCurrent < CameraZoomMinimumDistance)
	{
		//SET CameraZoomCurrent to CameraZoomMinimumDistance
		CameraZoomCurrent = CameraZoomMinimumDistance;
	}
	//ENDIF

	//SET CameraBoom->TargetArmLength to CameraZoomCurrent
	CameraBoom->TargetArmLength = CameraZoomCurrent;
}
void AMultiCamCharacter::ZoomCameraOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Zoom Camera Out Requested");
	//INCREMENT CameraZoomCurrent by CameraZoomIncrement
	CameraZoomCurrent += CameraZoomIncrement;
	//IF CameraZoomCurrent GREATER THAN CameraZoomMaximumDistance
	if (CameraZoomCurrent > CameraZoomMaximumDistance)
	{
		//SET CameraZoomCurrent to CameraZoomMaximumDistance
		CameraZoomCurrent = CameraZoomMaximumDistance;
	}
	//ENDIF

	//SET CameraBoom->TargetArmLength to CameraZoomCurrent
	CameraBoom->TargetArmLength = CameraZoomCurrent;
}


void AMultiCamCharacter::CycleCamera()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Cycle Camera Requested");
	//DECLARE a variable called newCameraMode and assign it to CameraModeEnum + 1
	int newCameraMode = CameraModeEnum + 1;

	//IF newCameraMode GREATER THAN OR EQUAL ECharacterCameraMode::COUNT
	if (newCameraMode >= ECharacterCameraMode::COUNT)
	{
		//SET newCameraMode to ECharacterCameraMode::ThirdPersonDefault
		newCameraMode = ECharacterCameraMode::ThirdPersonDefault;
	}
	//ENDIF

	//CALL SetCameraMode() passing in (ECharacterCameraMode::Type)newCameraMode
	SetCameraMode((ECharacterCameraMode::Type)newCameraMode);
}


void AMultiCamCharacter::SetCameraMode(ECharacterCameraMode::Type newCameraMode)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Setting Camera Mode to " + GetNameForCameraMode(newCameraMode));
	//SET CameraModeEnum to newCameraMode
	CameraModeEnum = newCameraMode;
	//CALL UpdateForCameraMode()
	UpdateForCameraMode();
	
}

void AMultiCamCharacter::TurnAtRate(float Rate)
{
	if (Rate == 0.f || Controller == nullptr) return;
	
	// calculate delta for this frame from the rate information
	//IF NOT bIsResetting
	if (!bIsResetting)
	{
		//CALL AddControllerYawInput() passing in Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		//SET LastMovementTime to FApp::GetCurrentTime()
		LastMovementTime = FApp::GetCurrentTime();
	}
	//ENDIF
}

void AMultiCamCharacter::LookUpAtRate(float Rate)
{
	
	if (Rate == 0.f || Controller == nullptr) return;

	//CALL AddControllerPitchInput() passing in Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	//SET LastMovementTime to FApp::GetCurrentTime()
	LastMovementTime = FApp::GetCurrentTime();
}

void AMultiCamCharacter::MoveForward(float Value)
{
	if (Value == 0.f || Controller == nullptr) return;
	
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);

	//SET LastMovementTime to FApp::GetCurrentTime()
	LastMovementTime = FApp::GetCurrentTime();
}

void AMultiCamCharacter::MoveRight(float Value)
{
	if (Value == 0.f || Controller == nullptr) return;
	
	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get right vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// add movement in that direction
	AddMovementInput(Direction, Value);

	//SET LastMovementTime = FApp::GetCurrentTime()
	LastMovementTime = FApp::GetCurrentTime();
}

void AMultiCamCharacter::HandleYawInput(float turnInput)
{
	if (!bIsResetting)
	{
		if (turnInput != 0.f)
		{
			AddControllerYawInput(turnInput);
			LastMovementTime = FApp::GetCurrentTime();
		}
	}
}

// TILAN: - Camera Mode
void AMultiCamCharacter::UpdateForCameraMode()
{
	/* Changes visibility of firstand third person meshes */

	//CALL SetCamera()
	SetCamera();

	//GET the PlayerController and Assign it to a variable called OurPlayerController, there are 3 ways to do this
	APlayerController* OurPlayerController = (APlayerController*)GetController();
	//IF OurPlayerController NOT nullptr
	if (OurPlayerController)
	{
		//CALL SetViewTargetWithBlend() on OurPlayerController passing in this, .25f, EViewTargetBlendFunction::VTBlend_EaseIn
		OurPlayerController->SetViewTargetWithBlend(this, .25f, EViewTargetBlendFunction::VTBlend_EaseIn);
	}
	//ENDIF
}

void AMultiCamCharacter::SetCamera()
{
	
	//SET bIsResetting to false
	bIsResetting = false;
	//CALL GetMesh()->SetOnlyOwnerSee() passing in false
	GetMesh()->SetOnlyOwnerSee(false);
	//CALL GetMesh()->MarkRenderStateDirty()
	GetMesh()->MarkRenderStateDirty();

	bool bThirdPersonCam = (CameraModeEnum == ECharacterCameraMode::ThirdPersonDefault || CameraModeEnum == ECharacterCameraMode::ThirdPersonReseting || CameraModeEnum == ECharacterCameraMode::ThirdPersonOverShoulder) ? true : false;
	if (bThirdPersonCam)//(CameraModeEnum == ECharacterCameraMode::ThirdPersonDefault || CameraModeEnum == ECharacterCameraMode::ThirdPersonReseting || CameraModeEnum == ECharacterCameraMode::ThirdPersonOverShoulder)
	{
		//SET bUseControllerRotationYaw to false
		//bUseControllerRotationYaw = false;
		//CALL GetMesh()->bOwnerNoSee to false
		GetMesh()->bOwnerNoSee = false;
		//CALL GetMesh()->MarkRenderStateDirty()
		GetMesh()->MarkRenderStateDirty();
		//CALL GetMesh()->SetVisibility() passing in true
		GetMesh()->SetVisibility(true);
		//CALL GetMesh()->MarkRenderStateDirty()
		GetMesh()->MarkRenderStateDirty();
		//CALL SetVisibility() on ArmsMesh passing in false
		ArmsMesh->SetVisibility(false);
		//CALL MarkRenderStateDirty() on ArmsMesh
		ArmsMesh->MarkRenderStateDirty();
		//CALL SetVisibility() on BodyMesh passing in false
		BodyMesh->SetVisibility(false);
		//CALL MarkRenderStateDirty() on BodyMesh
		BodyMesh->MarkRenderStateDirty();

		//IF CameraModeEnum IS ECharacterCameraMode::ThirdPersonDefault OR CameraModeEnum IS ECharacterCameraMode::ThirdPersonReseting
		if (CameraModeEnum == ECharacterCameraMode::ThirdPersonDefault || CameraModeEnum == ECharacterCameraMode::ThirdPersonReseting)
		{
			//SET GetCharacterMovement()->bOrientRotationToMovement to true
			GetCharacterMovement()->bOrientRotationToMovement = true;
			//CALL SetActiveCameraComponent() passing in FollowCamera
			SetActiveCameraComponent(FollowCamera);
		}
		//ELSE IF CameraModeEnum IS ECharacterCameraMode::ThirdPersonOverShoulder
		else if (CameraModeEnum == ECharacterCameraMode::ThirdPersonOverShoulder)
		{
			//bUseControllerRotationYaw = true;
			OverShoulderCameraBoom->bUsePawnControlRotation = true;
			//SET GetCharacterMovement()->bOrientRotationToMovement to FALSE
			GetCharacterMovement()->bOrientRotationToMovement = false;
			//CALL SetActiveCameraComponent() passing in OverShoulderCamera
			SetActiveCameraComponent(OverShoulderCamera);
		}
		//ENDIF

	}
	//ELSE IF CameraModeEnum IS ECharacterCameraMode::FirstPerson
	else 
	{
		//SET bUseControllerRotationYaw to true
		bUseControllerRotationYaw = true;
		//SET GetMesh()->bOwnerNoSee to true
		GetMesh()->bOwnerNoSee = true;                                              //COMMENT TO SEE WHAT HAPPENS
		//CALL GetMesh()->MarkRenderStateDirty()
		GetMesh()->MarkRenderStateDirty();
		//CALL SetVisibility() on ArmsMesh passing in true
		ArmsMesh->SetVisibility(true);
		//CALL MarkRenderStateDirty() on ArmsMesh
		ArmsMesh->MarkRenderStateDirty();
		//CALL SetVisibility() on BodyMesh passing in true
		BodyMesh->SetVisibility(true);
		//CALL SetOnlyOwnerSee() on BodyMesh passing in true
		BodyMesh->SetOnlyOwnerSee(true);
		//CALL MarkRenderStateDirty() on BodyMesh
		BodyMesh->MarkRenderStateDirty();
		//SET GetCharacterMovement()->bOrientRotationToMovement to false
		GetCharacterMovement()->bOrientRotationToMovement = false;
		//CALL SetActiveCameraComponent() passing in FirstPersonCamera
		SetActiveCameraComponent(FirstPersonCamera);
	}
	//ENDIF

	//IF CameraModeEnum IS ECharacterCameraMode::ThirdPersonOverShoulder OR CameraModeEnum IS ECharacterCameraMode::FirstPerson
	if (CameraModeEnum == ECharacterCameraMode::ThirdPersonOverShoulder || CameraModeEnum == ECharacterCameraMode::FirstPerson)
		//SET bIsResetting to false
		bIsResetting = false;
	//ENDIF

}

// TILAN: - Camera Animation
void AMultiCamCharacter::SetActiveCameraComponent(UCameraComponent *component)
{
	//CALL SetActive() on FollowCamera passing in false
	FollowCamera->SetActive(false);
	//CALL SetActive() on FirstPersonCamera passing in false
	FirstPersonCamera->SetActive(false);
	//CALL SetActive() on OverShoulderCamera passing in false
	OverShoulderCamera->SetActive(false);
	//CALL SetActive() on component passing in true
	component->SetActive(true);
}

UCameraComponent * AMultiCamCharacter::GetActiveCameraComponent()
{
	if (OverShoulderCamera->IsActive()) // bIsActive
	{
		return OverShoulderCamera;
	}
	if (FirstPersonCamera->IsActive()) //bIsActive
	{
		return FirstPersonCamera;
	}
	
	return FollowCamera;
}

// TILAN: - Tick
void AMultiCamCharacter::ResettingTick(float DeltaSeconds)
{
	//DECLARE a variable called Rotation of type const FRotator and ASSIGN it to the return value of Controller->GetControlRotation()
	FRotator Rotation = Controller->GetControlRotation();
	//DECLARE a variable called meshRotation of type FRotator and ASSIGN it to the return value of  GetMesh()->GetForwardVector().Rotation()
	FRotator meshRotation = GetMesh()->GetForwardVector().Rotation();
	//INCREMENT meshRotation.Yaw by 90.0f
	meshRotation.Yaw += 90.f;

	/*Delta between the mesh's Yaw rotation and the Yaw rotation of the Camera*/
	//DECLARE a variable called delta and assign it to the value of meshRotation.Yaw - Rotation.Yaw
	float delta = meshRotation.Yaw - Rotation.Yaw;
	
	// Prevent going "the long way around"
	//IF fabsf(delta) GREATER THAN OR EQUAL to 180.f
	if (fabsf(delta) >= 180.f)
	{
		//IF delta LESS THAN OR EQUAL TO 0
		if (delta <= 0)
		{
			//INCREMENT delta by 360
			delta += 360.f;
		}
		//ELSE
		else
			//DECREMENT delta by 360
		{
			delta -= 360.f;
		}
	
		//ENDIF
	}
	//ENDIF

	//IF fabsf(delta) LESS THAN OR EQUAL to 1.f
	if (fabsf(delta) <= 1.f)
	{
		//SET bIsResetting to false
		bIsResetting = false;
		//SET IsAutoReset to false
		IsAutoReset = false;
	}
	//ELSE
	else
	{
		//DECLARE a variable called resetSpeed of type float and ASSIGN it to the return of (IsAutoReset) ? AutoResetSpeed : CameraResetSpeed
		float resetSpeed = (IsAutoReset) ? AutoResetSpeed : CameraResetSpeed;
		//CALL AddControllerYawInput() passing in delta * DeltaSeconds * resetSpeed
		AddControllerYawInput(delta * DeltaSeconds * resetSpeed);
	}
	//ENDIF
}

void AMultiCamCharacter::ThirdPersonResetingTick(float DeltaSeconds)
{
	//DECLARE a variable called currentTime of type float and ASSIGN it to the return value of FApp::GetCurrentTime()
	float currentTime = FApp::GetCurrentTime();

	//IF currentTime GREATER THAN LastMovementTime + AutoResetDelaySeconds
	if (currentTime > LastMovementTime + AutoResetDelaySeconds)
	{
		//SET IsAutoReset to true
		IsAutoReset = true;
		//SET bIsResetting to true
		bIsResetting = true;
	}
	//ENDIF
	//IF bIsResetting
	if (bIsResetting)
	{
		//CALL ResettingTick() passing in DeltaSeconds
		ResettingTick(DeltaSeconds);
	}
	//ENDIF

	
}
void AMultiCamCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//IF CameraModeEnum IS EQUAL to ECharacterCameraMode::ThirdPersonReseting
	if (CameraModeEnum == ECharacterCameraMode::ThirdPersonReseting)
	{
		//CALL ThirdPersonResetingTick() passing in DeltaSeconds
		ThirdPersonResetingTick(DeltaSeconds);
	}
	//ENDIF
	
}


