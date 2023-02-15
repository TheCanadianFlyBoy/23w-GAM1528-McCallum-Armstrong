// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleDoorGameMode.h"
#include "SimpleDoorCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASimpleDoorGameMode::ASimpleDoorGameMode()
{
	// set default pawn class to our Blueprinted character Blueprint
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_SimpleDoorCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
