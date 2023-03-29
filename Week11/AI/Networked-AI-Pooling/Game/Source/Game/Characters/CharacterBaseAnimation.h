// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <Animation/AnimInstance.h>
#include "CharacterBaseAnimation.generated.h"

UCLASS()
class GAME_API UCharacterBaseAnimation : public UAnimInstance
{
	GENERATED_BODY()

public:
	//TODO: 8 NET CODE 2 SHOOTING
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bHasWeapon;

	//TODO: 9 NET CODE 2 SHOOTING
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsAiming;

	//TODO: 10 NET CODE 2 SHOOTING
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving;

	//TODO: 11 NET CODE 2 SHOOTING
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MovementSpeed;

	//TODO: 12 NET CODE 2 SHOOTING
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StrafingRotation;

protected:
	UCharacterBaseAnimation();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	APawn* OwningPawn;
};
