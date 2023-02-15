// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UseInterface.h"
#include "Door.generated.h"

class UAnimationAsset;

UCLASS()
class SIMPLEDOOR_API ADoor : public AActor, public IUseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	UPROPERTY(EditAnywhere, Category = "Week 3|Door")
	class USkeletalMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Week 3|Door")
	UAnimationAsset* OpenAnimation;
	UPROPERTY(EditAnywhere, Category = "Week 3|Door")
	UAnimationAsset* CloseAnimation;

	UPROPERTY(ReplicatedUsing = DoorToggled)
	bool bDoorOpen;

	UFUNCTION()
	void DoorToggled();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Use_Implementation() override;

	virtual void SomeFunc2() override;
};
