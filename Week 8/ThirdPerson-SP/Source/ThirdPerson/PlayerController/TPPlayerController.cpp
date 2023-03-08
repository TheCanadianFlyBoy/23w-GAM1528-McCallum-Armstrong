// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPlayerController.h"

#include "TPPlayerState.h"
#include "GameFramework/GameStateBase.h"

//NETWORK
void ATPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//METHOD 2 Set Player Name
	ATPPlayerState* PS = Cast<ATPPlayerState>(this->PlayerState);
	if (PS)
	   PS->SetName("Player - " + FString::FromInt(FMath::RandRange(1, 100)));
}
