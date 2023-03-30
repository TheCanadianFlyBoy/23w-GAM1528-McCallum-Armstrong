// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacterUE5Character.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

#include "Engine/StaticMeshActor.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	SpawnPoint = FVector(-800.f, -230.f, 300.f);
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	for (TActorIterator<AStaticMeshActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		ActorItr->SetReplicates(true);
		FRepMovement mv;
		mv.bRepPhysics = true;
		ActorItr->SetReplicatedMovement(mv);
		ActorItr->GetRootComponent()->SetIsReplicated(true);
	}

}

void AFPSGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);
	SpawnWeapon();
}

void AFPSGameMode::SpawnWeapon()
{
	const float WeaponOffset = 100.f;
	AActor* Weapon = GetWorld()->SpawnActor(WeaponTemplate, &SpawnPoint, &FRotator::ZeroRotator);
	Weapon->SetReplicates(true);
	SpawnPoint.X += WeaponOffset;
}

