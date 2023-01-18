// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MultiCamHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTICAM_API AMultiCamHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMultiCamHUD();
	virtual void DrawHUD() override;
	
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	
	
};
