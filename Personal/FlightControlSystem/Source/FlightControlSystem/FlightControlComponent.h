// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "FlightControlComponent.generated.h"

UENUM()
enum class EFlightModelType {
	Analog,
	Arcade
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTCONTROLSYSTEM_API UFlightControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlightControlComponent();

	//** SOUND **//
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		class UAudioComponent* AudioComponent;
	UPROPERTY(EditAnywhere, Category = "Sound")
		class USoundBase* EngineSound;

	//** ACTIONS **//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* PitchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RollAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* YawAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ThrottleAction;

	//** FLIGHT MODEL **//
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		EFlightModelType CurrentFlightModel;
	//Throttle
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float MinSpeed;
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float MaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float Acceleration;
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float Deceleration;
	//Axes of flight
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float PitchRate;
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float RollRate;
	UPROPERTY(EditAnywhere, Category = "Flight Model")
		float YawRate;
	//Input Storage
	FRotator FlightAmounts;
	FRotator FlightInputs;
	float ThrottleAmount;
	float ThrottleInput;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Flight Model Mode

	//Owner Pointers
	class UCharacterMovementComponent* CharacterMovement;
	class ACharacter* Owner;

	// AXES OF MOVEMENT
	void Pitch(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void Yaw(const FInputActionValue& Value);
	// THROTTLE
	void Throttle(const FInputActionValue& Value);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Setup movement
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

		
};
