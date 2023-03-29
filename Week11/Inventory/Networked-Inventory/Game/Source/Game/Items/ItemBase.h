// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/Actor.h>
#include "ItemBase.generated.h"

UCLASS()
class GAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
protected:
	AItemBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//TODO: WEEK11
	class UPrimitiveComponent* Primitive;

	virtual void PostInitializeComponents() override;

//TODO: WEEK11
public:
	
	UFUNCTION(NetMulticast, Reliable)
		virtual void NMC_Disable();

	UFUNCTION(NetMulticast, Reliable)
		virtual void NMC_Enable();

	void ShowSelect(bool selected);
	
};
