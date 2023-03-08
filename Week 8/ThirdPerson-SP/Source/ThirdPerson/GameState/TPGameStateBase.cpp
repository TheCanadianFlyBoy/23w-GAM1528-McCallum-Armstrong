// Fill out your copyright notice in the Description page of Project Settings.


#include "TPGameStateBase.h"
#include "Net/UnrealNetwork.h"

//NETWORK
ATPGameStateBase::ATPGameStateBase()
{
	SetReplicates(true);
	bAlwaysRelevant = true;
}

void ATPGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//REPLICATE  GameTime
	DOREPLIFETIME(ATPGameStateBase, GameTime);
}

void ATPGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	//NETWORK
	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle UpdateHandle;
		GetWorldTimerManager().SetTimer(UpdateHandle, this, &ATPGameStateBase::SetGameTime, 1.0f, true, 0.0f);
	}
}

void ATPGameStateBase::SetGameTime()
{
	GameTime += 1;
}