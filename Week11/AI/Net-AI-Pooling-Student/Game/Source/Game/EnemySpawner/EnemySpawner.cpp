// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include <AI\AICharacterBase.h>
#include <Engine.h>

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}
//TODO: WEEK11
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	//IF LocalRole IS Authority
	if(GetLocalRole() == ROLE_Authority)
	{
		//CALL UGameplayStatics::GetAllActorsWithTag() passing in GetWorld(), FName("SpawnPoints"), SpawnPoints
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SpawnPoints"), SpawnPoints);

		//DECLARE a variable called SpawnParams of type FActorSpawnParameters
		FActorSpawnParameters SpawnParams;
		//SET SpawnParams.Owner to this
		SpawnParams.Owner = this;
		//SET SpawnParams.SpawnCollisionHandlingOverride to ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		//IF EnempBP IS NOT nullptr
		if (EnemyBP)
		{
			//FOR size_t i = 0; i < ENEMY_POOL_SIZE; ++i
			for (size_t i = 0; i < ENEMY_POOL_SIZE; ++i)
			{
				//DECLARE a variable waitingZone of type FVector and Assign it to ENEMY_POOL_WAITING_ZONE * (i + 1)
				FVector WaitingZone = ENEMY_POOL_WAITING_ZONE * (i + 1);

				//DECLARE a variable called enemyTransform of type FTransform
				FTransform EnemyTransform;
				//CALL SetLocation() on enemyTransform passing in waitingZone
				EnemyTransform.SetLocation(WaitingZone);
				//CALL SetRotation() on enemyTransform FRotator::ZeroRotator.Quaternion()
				EnemyTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
				/* Lets you initialize stuff before the actor is constructed, then you have to call FinishSpawning or UGameplayStatics::FinishSpawningActor to finalize the spawning https://docs.unrealengine.com/en-US/API/Runtime/Engine/Engine/UWorld/SpawnActorDeferred/index.html */
				//DECLARE a variable called newEnemy of type AAICharacterBase* using SpawnActorDeferred() like this GetWorld()->SpawnActorDeferred<AAICharacterBase>(EnemyBP, enemyTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn)
				AAICharacterBase* NewEnemy = GetWorld()->SpawnActorDeferred<AAICharacterBase>(EnemyBP, EnemyTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

				//CALL SetOwner() on newEnemy passing in this
				NewEnemy->SetOwner(this); //??why??
				//SET newEnemy->waitingZone to waitingZone
				NewEnemy->waitingZone = WaitingZone;
				//SET newEnemy->spawner to this
				NewEnemy->spawner = this;

				//CALL Enqueue() on EnemyPool passing in newEnemy
				EnemyPool.Enqueue(NewEnemy);

				//CALL FinishSpawning() on newEnemy passing in enemyTransform
				NewEnemy->FinishSpawning(EnemyTransform);
				//OR
				//UGameplayStatics::FinishSpawningActor(newEnemy, enemyTransform);
			}
			//ENDFOR
		}
		//ENDIF
		
		//DECLARE a variable called SpawnTimer of type FTimerHandle
		FTimerHandle SpawnTimer;
		//DECLARE a variable called TimerManager of type auto& and assign it to the return value of GetWorld()->GetTimerManager()
		auto& TimerManager = GetWorld()->GetTimerManager();
		//CALL SetTimer() on TimerManager passing in SpawnTimer, this, &AEnemySpawner::Spawn, 3, true
		TimerManager.SetTimer(SpawnTimer, this, &AEnemySpawner::Spawn, 3, true);
	}
	//ENDIF

}

//TODO: WEEK11
void AEnemySpawner::Spawn()
{
	//IF LocalRole is Authority
	if (GetLocalRole() == ROLE_Authority)
	{
		//IF NOT EnemyPool.IsEmpty()
		if (!EnemyPool.IsEmpty())
		{
			//DECLARE a variable called nextEnemy of type AAICharacterBase*
			AAICharacterBase* NextEnemy;
			//CALL Peek() on EnemyPool passing in nextEnemy
			EnemyPool.Peek(NextEnemy);
			//IF nextEnemy IS NOT nullptr
			if (NextEnemy)
			{
				//IF EnemyPool.Pop()
				if (EnemyPool.Pop())
				{
					//IF nextEnemy->IsValidLowLevel()
					if (NextEnemy->IsValidLowLevel())
					{
						//CALL SetActorLocation() on nextEnemy passing in SpawnPoints[NextSpawn++]->GetActorLocation()
						NextEnemy->SetActorLocation(SpawnPoints[NextSpawn++]->GetActorLocation());
						//CALL Reset() on nextEnemy
						NextEnemy->Reset();
						//IF NextSpawn IS EQUAL SpawnPoints.Num()
						if (NextSpawn == SpawnPoints.Num())
							//SET NextSpawn to 0
							NextSpawn = 0;
						//ENDIF
						
						//INCREMENT Count
						Count++;
					}
					//ELSE
					else
					{
						//CALL GEngine->AddOnScreenDebugMessage() passing in -1, 3, FColor::Red, "Invalid")
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Invalid");
					}
					//ENDIF

				}
				//ENDIF
			}
			//ENDIF
		}
		//ENDIF
	}
	//ENDIF
}

//TODO: WEEK11
void AEnemySpawner::ReleaseEnemy(AAICharacterBase* enemy)
{
	//IF LocalRole IS Authority
	if (GetLocalRole() == ROLE_Authority)
	{
		//CALL SetActorLocation() on enemy passing in enemy->waitingZone
		enemy->SetActorLocation(enemy->waitingZone);
		//DECREMENT Count
		Count--;
		//CALL EnemyPool.Enqueue() passing in enemy
		EnemyPool.Enqueue(enemy);
	}
	//ENDIF
}


void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Purple, "Count - " + FString::FromInt(Count));
}

