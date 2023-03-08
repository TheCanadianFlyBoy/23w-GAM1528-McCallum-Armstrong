// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATPPlayerState();

	//NETWORK
	FString PlayerName;

	//NETWORK
	void SetName(const FString& Name);
protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;


};
