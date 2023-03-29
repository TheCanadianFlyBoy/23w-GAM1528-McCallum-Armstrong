// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/PlayerController.h>
#include "PlayerControllerDefault.generated.h"

UCLASS()
class GAME_API APlayerControllerDefault : public APlayerController
{
	GENERATED_BODY()
	
protected:
	APlayerControllerDefault();

	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* Pawn) override;
	virtual void BeginPlay() override;

	//TODO: WEEK11
	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		bool bRespawnImmediately;

	//TODO: WEEK11
	/* Start spectating. Should be called only on server */
	void StartSpectating();


		
private:
	void MoveVertical(float Scale);
	void MoveHorizontal(float Scale);

	void FirePressed();
	void FireReleased();
	void AimPressed();
	void AimReleased();

	void InteractPressed();

	void Restart();

	//TODO: WEEK11
	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

private:
	class ACharacterBase* Character;
};
