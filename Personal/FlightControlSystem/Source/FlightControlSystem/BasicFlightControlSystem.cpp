// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicFlightControlSystem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FlightControlComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
ABasicFlightControlSystem::ABasicFlightControlSystem()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Attach mesh to root
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	
	//Flight Control
	FlightControl = CreateDefaultSubobject<UFlightControlComponent>(TEXT("FlightControl"));
	FlightControl->AudioComponent->SetupAttachment(RootComponent);

	//Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->AddLocalOffset(FVector(0, -20, 5));


}

// Called when the game starts or when spawned
void ABasicFlightControlSystem::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ABasicFlightControlSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasicFlightControlSystem::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	FlightControl->SetupPlayerInputComponent(PlayerInputComponent);


}

