// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class UE5_LAB2_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
		class UParticleSystem* ParticleSystem;

	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		class URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditAnywhere, Category = "Grenade")
		float DamageRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Grenade")
		float BaseDamage = 10.f;

	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		class UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, Category = "Grenade")
		class USoundBase* ExplosionSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle ExplosiveTimer;

	UFUNCTION(Reliable, NetMulticast)
		void Explode();
	float RotationSpeed = 1.0f;
	bool bArmed = false;

	UFUNCTION()
		void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
