// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class UE5_LAB1_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	//Base sphere component
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
		class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		class USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		class UParticleSystem* ProjectileFX;


	UPROPERTY(EditAnywhere, Category = "Projectile");
		float ProjectileSpeed;
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
		class UProjectileMovementComponent* MovementComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
