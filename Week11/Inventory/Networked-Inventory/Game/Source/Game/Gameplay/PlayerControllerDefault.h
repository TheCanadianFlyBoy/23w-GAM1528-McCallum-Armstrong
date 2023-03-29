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

	//TODO: WEEK8
	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		bool bRespawnImmediately;

	//TODO: WEEK8
	/* Start spectating. Should be called only on server */
	void StartSpectating();

	virtual void BeginSpectatingState() override;

	//TODO: WEEK11
	UPROPERTY(EditAnywhere, Category = Debug)
		TSubclassOf<class UDebugWidget> DebugMenuClass;
		
private:
	void MoveVertical(float Scale);
	void MoveHorizontal(float Scale);

	void FirePressed();
	void FireReleased();
	void AimPressed();
	void AimReleased();

	void NextInventoryItem();
	void PrevInventoryItem();
	
	void InteractPressed();

	void Restart();

	//TODO: WEEK8
	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	//TODO: WEEK11
	void Handle_DebugMenuPressed();
	void CreateDebugWidget();

	void ShowDebugMenu();
	void HideDebugMenu();

	UPROPERTY(Transient)
		UDebugWidget* DebugMenuInstance = nullptr;

	UPROPERTY(EditAnywhere)
		bool bShowDebugMenu = false;

private:
	class ACharacterBase* Character;
};
