// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightControlComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IAudioExtensionPlugin.h"


// Sets default values for this component's properties
UFlightControlComponent::UFlightControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Enter defaults
	FlightInputs = FRotator(0.f);
	FlightAmounts = FRotator(0.f);

	PitchRate = 35.f;
	YawRate = 25.f;
	RollRate = 45.f;

	MinSpeed = 1200.f;
	MaxSpeed = 9000.f;
	Acceleration = 25.f;
	Deceleration = 17.f;
	ThrottleAmount = MinSpeed;

	//Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = true;
	AudioComponent->SetSound(EngineSound);

	CurrentFlightModel = EFlightModelType::Analog;

	// ...
}


// Called when the game starts
void UFlightControlComponent::BeginPlay()
{
	Super::BeginPlay();

	//Attempt to setup inputs
	if (Cast<ACharacter>(GetOwner())) {
		Owner = (ACharacter*)(GetOwner());
		CharacterMovement = Owner->GetCharacterMovement();

		Owner->bUseControllerRotationPitch = false;
		Owner->bUseControllerRotationRoll = false;
		Owner->bUseControllerRotationYaw = false;
		//Prep sound
		

		//Setup basic movement
		CharacterMovement->SetJumpAllowed(false);
		CharacterMovement->MaxFlySpeed = MaxSpeed;
	}
	// ...
	
}

void UFlightControlComponent::Pitch(const FInputActionValue& Value)
{
	FlightInputs.Pitch = Value.Get<float>();	
}

void UFlightControlComponent::Yaw(const FInputActionValue& Value)
{
	FlightInputs.Yaw = Value.Get<float>();
}

void UFlightControlComponent::Roll(const FInputActionValue& Value)
{
	FlightInputs.Roll = Value.Get<float>();
}

void UFlightControlComponent::Throttle(const FInputActionValue& Value)
{
	
	ThrottleInput = Value.Get<float>();
	//AudioComponent->Play();
}



// Called every frame
void UFlightControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Debug
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, "Throttle" + FString::SanitizeFloat(ThrottleAmount));
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, "Rotation" + FlightAmounts.ToCompactString());

	//Declare Rotators for later usage
	FRotator PitchRotator, RollRotator, YawRotator;
	switch (CurrentFlightModel)
	{
		//** ANALOG FLIGHT MODEL **//
		case EFlightModelType::Analog:
		{
			//Throttle calculations
			ThrottleAmount = FMath::Clamp(ThrottleAmount + ThrottleInput * Acceleration, MinSpeed, MaxSpeed); //SYNC TO DELTA TIME
			//Rotation calculation
			FlightAmounts = FRotator	(	FMath::Lerp(FlightAmounts.Pitch, PitchRate * FlightInputs.Pitch, DeltaTime),
											FMath::Lerp(FlightAmounts.Yaw, PitchRate * FlightInputs.Yaw, DeltaTime),
											FMath::Lerp(FlightAmounts.Roll, PitchRate * FlightInputs.Roll, DeltaTime));
			break;
		}
		//** ARCADE FLIGHT MODEL **//
		case EFlightModelType::Arcade:
		{
			//Throttle Calculations
			float DeltaThrottle = MaxSpeed - MinSpeed / 2;
			if (ThrottleInput > 0)
			{
				ThrottleAmount = FMath::Lerp(ThrottleAmount, MaxSpeed, DeltaTime);
			}
			else if (ThrottleInput < 0)
			{
				ThrottleAmount = FMath::Lerp(ThrottleAmount, MinSpeed, DeltaTime);
			}
			else {
				ThrottleAmount = FMath::Lerp(ThrottleAmount, DeltaThrottle, DeltaTime);
			}
			//Rotation calculations
			FlightAmounts = FRotator(FMath::Lerp(FlightAmounts.Pitch, PitchRate * FlightInputs.Pitch, DeltaTime),
									 FMath::Lerp(FlightAmounts.Yaw, PitchRate * FlightInputs.Yaw, DeltaTime),
									 FMath::Lerp(FlightAmounts.Roll, PitchRate * FlightInputs.Roll, DeltaTime));
			break;
		}
	}
	//Commit to thrust
	Owner->AddActorLocalOffset(FVector(ThrottleAmount * DeltaTime, 0, 0));
	//Commit to rotation
	PitchRotator = FRotator(			FlightAmounts.Pitch * DeltaTime * MinSpeed*2/ThrottleAmount, 0,	0);
	RollRotator = FRotator(0.f, 0.f,	FlightAmounts.Roll * DeltaTime	* MinSpeed*2/ThrottleAmount);
	YawRotator = FRotator(0.f,			FlightAmounts.Yaw * DeltaTime * MinSpeed*2/ThrottleAmount	, 0.f);
	//Add rotation
	Owner->AddActorLocalRotation(PitchRotator);
	Owner->AddActorLocalRotation(RollRotator);
	Owner->AddActorLocalRotation(YawRotator);
	//Set audio pitch
	AudioComponent->SetPitchMultiplier(ThrottleAmount / MaxSpeed * 2);
}

//Called to setup inputs
void UFlightControlComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		//AXES OF MOVEMENT
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &UFlightControlComponent::Pitch);
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Completed, this, &UFlightControlComponent::Pitch);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &UFlightControlComponent::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &UFlightControlComponent::Roll);

		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &UFlightControlComponent::Yaw);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Completed , this, &UFlightControlComponent::Yaw);

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &UFlightControlComponent::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &UFlightControlComponent::Throttle);
	}
}

