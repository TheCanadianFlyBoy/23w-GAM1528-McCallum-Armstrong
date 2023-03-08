// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TPGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATPGameStateBase();
	
	UPROPERTY(Replicated)
		int GameTime;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

protected:


	virtual void BeginPlay() override;

	void SetGameTime();
};
