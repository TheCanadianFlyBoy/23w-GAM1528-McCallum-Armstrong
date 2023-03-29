// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/GameModeBase.h>
#include "GameModeDefault.generated.h"

UCLASS()
class GAME_API AGameModeDefault : public AGameModeBase
{
	GENERATED_BODY()

protected:
	AGameModeDefault();
	
	virtual void BeginPlay() override;
};
