// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class UE5_LAB2_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
		class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
		class UTextRenderComponent* TextComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void RestoreGrenades(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
