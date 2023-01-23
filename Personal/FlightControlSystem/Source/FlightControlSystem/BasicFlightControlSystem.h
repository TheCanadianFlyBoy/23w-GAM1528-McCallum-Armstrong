// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicFlightControlSystem.generated.h"

UCLASS()
class FLIGHTCONTROLSYSTEM_API ABasicFlightControlSystem : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicFlightControlSystem();

	UPROPERTY(EditDefaultsOnly, Category = "Fighter Mesh")
		class UStaticMeshComponent* MeshComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
		class UFlightControlComponent* FlightControl;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
