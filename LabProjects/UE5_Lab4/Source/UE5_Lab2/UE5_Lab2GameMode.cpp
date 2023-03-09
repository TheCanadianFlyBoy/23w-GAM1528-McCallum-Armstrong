// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_Lab2GameMode.h"
#include "UE5_Lab2Character.h"
#include "UObject/ConstructorHelpers.h"

AUE5_Lab2GameMode::AUE5_Lab2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
