// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBombActor.generated.h"

UCLASS()
class FPSGAME_API AFPSBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBombActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		float ExplodeDelay = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		class UParticleSystem* ExplosionTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		class USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		class UStaticMeshComponent* BombMesh;
	UPROPERTY(VisibleAnywhere, Category = "BombActor")
		class UBoxComponent* BombBox;
	UPROPERTY(VisibleAnywhere, Category = "BombActor")
		class URadialForceComponent* RadialForceComponent;
	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		TSubclassOf<UDamageType> DamageType;

	FTimerHandle FuseTimer;

	UFUNCTION()
		void Explode();
	UFUNCTION()
		void DestroyBomb();
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Hold(USkeletalMeshComponent* HoldingComponent);
	void Throw(FVector Direction);
};
