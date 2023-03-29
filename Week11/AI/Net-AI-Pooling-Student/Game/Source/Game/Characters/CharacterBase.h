// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

UCLASS()
class GAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
	float MaxJogSpeed;

	UPROPERTY(EditDefaultsOnly)
	float MaxPickUpDistance;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireHipAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireAimAnimation;

	UFUNCTION(BlueprintPure) //Like Declaring it as const. Return value cannot be changed in BP. Callable in Level BP's and regular BP's
	class AWeaponBase* GetCurrentWeapon();

	void Move(FVector Direction, float Scale);
	void Fire(bool Toggle);
	void Aim(bool Toggle);

	void Interact(AActor* Actor);

	class USkeletalMeshComponent* GetSkeletalMesh();


	//TODO: WEEK11
	void ApplyDamage(AActor* DamageCauser);

	FORCEINLINE class UHealthComponent* GetHealth(){ return Health;}

protected:
	ACharacterBase();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//TODO: 2 NET CODE 2 SHOOTING
	UFUNCTION()
	void OnWeaponFired();
	UFUNCTION(NetMulticast, Unreliable)
	void NMC_PlayWeaponFiringAnimation();

	//TODO: 3 NET CODE 2 SHOOTING
	UFUNCTION()
	virtual void OnDeath(AActor* KilledBy);
	UFUNCTION(NetMulticast, Reliable)
		void NMC_OnClientDeath();

	//TODO: NET CODE 1 NET PICKUP
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_HoldWeapon(class AWeaponBase* Weapon);

	//TODO: NET CODE 1 NET PICKUP
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_DropWeapon();

	//TODO: NET CODE 1 NET PICKUP
	UPROPERTY(Replicated)
		class AWeaponBase* CurrentWeapon;
///////////////////////////////////////////////////////////////////////////
private:


	
	//TODO: NET CODE 1 NET PICKUP
	void AttachCurrentWeapon();

	//TODO: 4 NET CODE 2 SHOOTING
	UPROPERTY(EditAnywhere, Replicated, Category = Movement)
		FVector CurrentFacing;
	/** Change the facing of the character */
	void ChangeFacing(FVector TargetVector);
	/** Change the facing of the character on server */
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_ChangeFacing(FVector TargetVector);

	//TODO: 5 NET CODE 2 SHOOTING
	UPROPERTY( Replicated)
	bool bIsAiming;
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SetIsAiming(bool val);

	//TODO: 6 NET CODE 2 SHOOTING
	UPROPERTY( Replicated)
	bool bIsFiring;	
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SetIsFiring(bool val);

	//TODO: 7 NET CODE 2 SHOOTING
	UPROPERTY( Replicated)
	bool bHasWeapon;
	void SetHasWeapon(bool val);

///////////////////////////////////////////////////////////////////////

	class UHealthComponent* Health;
	
	class USkeletalMeshComponent* SkeletalMesh;
	class UCharacterBaseAnimation* AnimationInstance;


};
