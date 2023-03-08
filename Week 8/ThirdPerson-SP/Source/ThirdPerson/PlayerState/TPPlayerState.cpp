// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

ATPPlayerState::ATPPlayerState()
{
	//NETWORK
	bAlwaysRelevant = true;
	SetReplicates(true);
}

void ATPPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ATPPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
//NETWORK
void ATPPlayerState::SetName(const FString& Name)
{
	
		PlayerName = Name;
}

