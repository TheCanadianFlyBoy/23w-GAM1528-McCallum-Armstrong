// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/Actor.h>
#include "Items/ItemBase.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFiredSignature);

UCLASS()
class GAME_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
	//TODO: 14 NET CODE 2 SHOOTING
	UPROPERTY(Replicated, EditDefaultsOnly)
	float FireRate;
	//TODO: 15 NET CODE 2 SHOOTING
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int MaximumAmmo;
	//TODO: 16 NET CODE 2 SHOOTING
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int CurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
	class UClass* ProjectileType;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponFiredSignature OnWeaponFired;
	
public:
	void Attach(class ACharacterBase* Character);
	void Detach();

	void PullTrigger();
	void ReleaseTrigger();

	FVector GetMuzzleLocation() const;

protected:
	AWeaponBase();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Fire();
	//TODO: 17 NET CODE 2 SHOOTING
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Fire();

private:
	void ClearFireTimer();
	FTimerHandle FireTimer;

private:
	class UPrimitiveComponent* Primitive;
	class UArrowComponent* Muzzle;
};
