// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "PlayerControllerDefault.h"

#include "UserWidget.h"
#include "Characters/CharacterBase.h"
#include "Items/ItemBase.h"
#include "NetworkSettings/DebugWidget.h"
APlayerControllerDefault::APlayerControllerDefault()
{
	bShowMouseCursor = true;

	bRespawnImmediately = false;
}

void APlayerControllerDefault::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveVertical", this, &APlayerControllerDefault::MoveVertical);
	InputComponent->BindAxis("MoveHorizontal", this, &APlayerControllerDefault::MoveHorizontal);

	InputComponent->BindAction("Fire", IE_Pressed, this, &APlayerControllerDefault::FirePressed);
	InputComponent->BindAction("Fire", IE_Released, this, &APlayerControllerDefault::FireReleased);

	InputComponent->BindAction("Aim", IE_Pressed, this, &APlayerControllerDefault::AimPressed);
	InputComponent->BindAction("Aim", IE_Released, this, &APlayerControllerDefault::AimReleased);

	InputComponent->BindAction("Interact", IE_Pressed, this, &APlayerControllerDefault::InteractPressed);

	//TODO: WEEK8
	InputComponent->BindAction("Restart", IE_Pressed, this, &APlayerControllerDefault::Restart);
}

void APlayerControllerDefault::AcknowledgePossession(APawn* PossesedPawn)
{
	Super::AcknowledgePossession(PossesedPawn);

	Character = Cast<ACharacterBase>(PossesedPawn);
	check(Character != nullptr && "Possesed a pawn that isn't of a desired character type!");

	//TODO: WEEK11
	InputComponent->BindAction("NextInventoryItem", IE_Pressed, this, &APlayerControllerDefault::NextInventoryItem);
	InputComponent->BindAction("PrevInventoryItem", IE_Pressed, this, &APlayerControllerDefault::PrevInventoryItem);
	InputComponent->BindAction("DropItem", IE_Pressed, Character, &ACharacterBase::InteractDropWeapon);

	InputComponent->BindAction("DebugMenu", IE_Pressed, this, &APlayerControllerDefault::Handle_DebugMenuPressed);
}

void APlayerControllerDefault::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerControllerDefault::MoveVertical(float Scale)
{
	if(!Character)
		return;

	Character->Move(FVector(1.0f, 0.0f, 0.0f), Scale);
}

void APlayerControllerDefault::MoveHorizontal(float Scale)
{
	if(!Character)
		return;
	
	Character->Move(FVector(0.0f, 1.0f, 0.0f), Scale);
}

void APlayerControllerDefault::FirePressed()
{
	if(!Character)
		return;

	Character->Fire(true);
}

void APlayerControllerDefault::FireReleased()
{
	if(!Character)
		return;

	Character->Fire(false);
}

void APlayerControllerDefault::AimPressed()
{
	if(!Character)
		return;
	
	Character->Aim(true);
}

void APlayerControllerDefault::AimReleased()
{
	if(!Character)
		return;

	Character->Aim(false);
}

//TODO: WEEK11
void APlayerControllerDefault::NextInventoryItem()
{
	//CALL Server_NextInventoryItem() on Character
	Character->Server_NextInventoryItem();
}
//TODO: WEEK11
void APlayerControllerDefault::PrevInventoryItem()
{
	//CALL Server_PrevInventoryItem() on Character
	Character->Server_PrevInventoryItem();
}

void APlayerControllerDefault::InteractPressed()
{
	if(!Character)
		return;

	// Get the actor under the cursor.
	FHitResult TraceResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, TraceResult);

	// Interact with the actor under the cursor.
	// We are fine with getting nullptr as the actor.
	Character->Interact(TraceResult.GetActor());
}

//TODO: WEEK8
void APlayerControllerDefault::Restart()
{
	//CALL ServerRestartPlayer()
	ServerRestartPlayer();
}

//TODO: WEEK8
void APlayerControllerDefault::UnFreeze()
{
	Super::UnFreeze();

	/* Respawn or spectate */
	//IF bRespawnImmediately IS true
	if (bRespawnImmediately)
	{
		//CALL ServerRestartPlayer()
		ServerRestartPlayer();
	}
	//ELSE
	else
	{
		//CALL StartSpectating()
		StartSpectating();
	}
	//ENDIF

}

//TODO: WEEK8
void APlayerControllerDefault::StartSpectating()
{
	/* Update the state on server */
	//CALL SetIsSpectator() on PlayerState passing in true
	PlayerState->SetIsSpectator(true);
	
	/* Waiting to respawn */
	//SET bPlayerIsWaiting to true
	bPlayerIsWaiting = true;

	/*Change the State On Server*/
	//CALL ChangeState() passing in NAME_Spectating
	ChangeState(NAME_Spectating);
	
	/* Push the state update to the client */
	//CALL ClientGotoState() passing in NAME_Spectating
	ClientGotoState(NAME_Spectating);

	/* Focus on the remaining alive player */
	//CALL ViewAPlayer() passing in 1
	//ViewAPlayer(1);
	//OR
	//CALL ServerViewNextPlayer()
	ServerViewNextPlayer();
	//SetSpectatorPawn(Character);
	
}
//TODO: WEEK8 - Good for Dedicated Server
void APlayerControllerDefault::BeginSpectatingState()
{
	Super::BeginSpectatingState();

}

//TODO: WEEK11 - DEBUG MENU
void APlayerControllerDefault::CreateDebugWidget()
{
	if (DebugMenuClass == nullptr)
	{
		return;
	}

	if (DebugMenuInstance == nullptr)
	{
		DebugMenuInstance = CreateWidget<UDebugWidget>(GetWorld(), DebugMenuClass);
		DebugMenuInstance->AddToViewport();
	}
}

void APlayerControllerDefault::Handle_DebugMenuPressed()
{
	bShowDebugMenu = !bShowDebugMenu;

	if (bShowDebugMenu)
	{
		ShowDebugMenu();
	}
	else
	{
		HideDebugMenu();
	}
}
void APlayerControllerDefault::ShowDebugMenu()
{
	CreateDebugWidget();

	if (DebugMenuInstance == nullptr)
	{
		return;
	}

	DebugMenuInstance->SetVisibility(ESlateVisibility::Visible);
	DebugMenuInstance->BP_OnShowWidget();
}

void APlayerControllerDefault::HideDebugMenu()
{
	if (DebugMenuInstance == nullptr)
	{
		return;
	}

	DebugMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
	DebugMenuInstance->BP_OnHideWidget();
}