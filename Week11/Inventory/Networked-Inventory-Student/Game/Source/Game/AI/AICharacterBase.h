// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AICharacterBase.generated.h"

//TODO: WEEK8 - Study Class Declaration

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrol,
	Suspicious,
	Alerted,
	Attack
};

UCLASS()
class GAME_API AAICharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase();

protected:
	virtual void BeginPlay() override;

	//Add AIModule to Build.cs deps
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	/*Event Callback when Pawn Is Seen*/
	UFUNCTION()//Must be a UFUnc otherwise we cant bind this function
		void OnPawnSeen(APawn* SeenPawn);

	/*Event Callback when Pawn Is Heard*/
	UFUNCTION()
		void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	/*Resets the Orientation to OriginalRotator*/
	UFUNCTION()
		void ResetOrientation();

	/*Resets AI back to patrol state*/
	UFUNCTION()
		void ResetPatrol();

	/*Gets called when GuardState changes, but only on the clients*/
	UFUNCTION()
		void OnRep_GuardState(); 

	/*Sets the AI's Guard State*/
	void SetGuardState(EAIState NewState);

	/*Can be used in Blueprint*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		void OnStateChanged(EAIState NewState);


	/*Sets the CurrentPatrol Point*/
	void MoveToNextPatrolPoint();

	/*Event Callback for OnDeath*/
	virtual void OnDeath(AActor* KilledBy) override;

	/*The Original Rotation of this Actor*/
	FRotator OriginalRotator;

	/*Timer to reset orientation*/
	FTimerHandle TimerHandle_ResetOrientation;
	/*Timer to reset patrol*/
	FTimerHandle TimerHandle_ResetPatrol;

	/*The AI's State */
	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
		EAIState GuardState;

	/* Flag for patrol or not */
	UPROPERTY(EditInstanceOnly, Category = "AI")
		bool bPatrol;


	/*TArray of Patrol Points*/
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
		TArray<AActor*> PatrolPoints;

	/*The Current Patrol Point to go to*/
	AActor* CurrentPatrolPoint;

	/*The AI's Weapon Blueprint to Spawn in BeginPlay()*/
	TSubclassOf<class AWeaponBase> WeaponBlueprint;


	/* Last time the player was spotted */
	UPROPERTY(Replicated)
	float LastSeenTime;

	/* Last time the player was heard */
	UPROPERTY(Replicated)
	float LastHeardTime;

	/* Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float SenseTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	UPROPERTY(Replicated)
	bool bSensedTarget;


	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*Resets this AI, Called in EnemySpawner*/
	void Reset();

	/* Is AI Active or not*/
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Enemy stats")
		bool isActive = false;

	/*The Enemy Spawner*/
	class AEnemySpawner* spawner = nullptr;

	/*The waiting zone*/
	FVector waitingZone;

private:
	/*The target to attack*/
	AActor* TargetActor;

};
