// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_LAB2_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

		virtual void BeginPlay() override;

public:
	UPROPERTY()
		class UCustomUserWidget* GameInfoWidget;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UCustomUserWidget> GameInfoBP;
		
};
