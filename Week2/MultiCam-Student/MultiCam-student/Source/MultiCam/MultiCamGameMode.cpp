// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MultiCam.h"
#include "MultiCamGameMode.h"
#include "MultiCamHUD.h"
#include "MultiCamCharacter.h"
#include "MultiCamPlayerController.h"
AMultiCamGameMode::AMultiCamGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_MultiCamCharacter"));
	
	PlayerControllerClass = AMultiCamPlayerController::StaticClass();
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		
		HUDClass = AMultiCamHUD::StaticClass();
	}
}
