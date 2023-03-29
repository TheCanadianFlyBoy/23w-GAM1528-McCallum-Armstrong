// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Queue.h"
#include "EnemySpawner.generated.h"

#define ENEMY_POOL_SIZE 10
#define ENEMY_POOL_WAITING_ZONE FVector(20000.f, 20000.f, 20000.f)

class AAICharacterBase;

//TODO: WEEK11 - Study Class Declaration

UCLASS()
class GAME_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

private:
	/*TArray of Spawn Points*/
	TArray<AActor*> SpawnPoints;

	/*The Enemy Pool Queue*/
	TQueue <AAICharacterBase*> EnemyPool;

	/*Unsigned in for NextSpawn Index*/
	size_t NextSpawn = 0;

	/*The Count of AI currently active*/
	int Count = 0;


public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	/*Releases the enemy, Puts it back in the queue and moves it to the waiting zone*/
	void ReleaseEnemy(AAICharacterBase* enemy);

	/*The Enemy BP for this Spawner*/
	UPROPERTY(EditDefaultsOnly, Category = "Enemy blueprint")
		TSubclassOf<AAICharacterBase> EnemyBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Removes the next available enemy from the Pool and puts it in play*/
	UFUNCTION()
	void Spawn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
