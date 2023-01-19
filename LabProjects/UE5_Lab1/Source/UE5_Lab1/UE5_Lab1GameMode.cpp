// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_Lab1GameMode.h"
#include "UE5_Lab1Character.h"
#include "UObject/ConstructorHelpers.h"

AUE5_Lab1GameMode::AUE5_Lab1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
