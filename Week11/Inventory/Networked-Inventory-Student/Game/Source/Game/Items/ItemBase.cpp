// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "ItemBase.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//TODO: WEEK11
void AItemBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get the root primitive component.
	Primitive = Cast<UPrimitiveComponent>(GetRootComponent());
}

void AItemBase::NMC_Enable()
{
	if (Primitive)
	{
		this->SetActorHiddenInGame(false);
		Primitive->SetActive(true);

	}
}

void AItemBase::NMC_Disable()
{
	if (Primitive)
	{
		this->SetActorHiddenInGame(true);
		Primitive->SetActive(false);
	}
}

//TODO: WEEK11
void AItemBase::ShowSelect(bool selected)
{
	//IF Primitive
	
		//CALL SetRenderCustomDepth() on Primitive passing in selected
	
	//ENDIF
}