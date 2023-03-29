// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "AICharacterBase.h"
#include "NavigationSystem.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h" //needed for DOREPLIFETIME
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"

#include "Weapons/WeaponBase.h"
#include "Gameplay/Health/HealthComponent.h"
#include "EnemySpawner/EnemySpawner.h"

#include "Util/BPFunctionLibrary.h"

//TODO: WEEK11
AAICharacterBase::AAICharacterBase() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	//IF LocalRole IS Authority
	if (GetLocalRole() == ROLE_Authority)
	{
			//CREATE the Pawn Sensing Component
		PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));

			//SUBSCRIBE to the OnSeePawn event on the PawnSensingComp passing in this, &AAICharacterBase::OnPawnSeen
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnPawnSeen);
	

			//SUBSCRIBE to the OnHearNoise event on the PawnSensingComp passing in this, &AAICharacterBase::OnNoiseHeard
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnNoiseHeard);

			//SET GuardState to EAIState::Patrol
		GuardState = EAIState::Patrol;
	
	}
	//ENDIF

	//LOAD a Weapon Blueprint
	static ConstructorHelpers::FClassFinder<AWeaponBase> ItemBlueprint(TEXT("/Game/Weapons/Base/Blueprints/BP_WeaponBase"));
	if (ItemBlueprint.Class) {
		WeaponBlueprint = ItemBlueprint.Class;
	}

	//Empty the Tags array
    Tags.Empty();
	//Add AICharacter tag to this Actor
    Tags.Add("AICharacter");

	//SET SenseTimeOut to 2.5f
	SenseTimeOut = 2.5f;
}

//TODO: WEEK11
void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//SET the OriginalRotator to the return of  GetActorRotation()
	OriginalRotator = GetActorRotation();

	//IF LocalRole IS Authority
	if (GetLocalRole() == ROLE_Authority)
	{
		// Spawn the Weapon and attach.
		//DECLARE a variable called SpawnParams
		FActorSpawnParameters SpawnParams;
		//SET SpawnParams.Instigator to this
		SpawnParams.Instigator = this;
		//SET SpawnParams.SpawnCollisionHandlingOverride to ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//IF WeaponBlueprint NOT nullptr
		if (WeaponBlueprint)
		{
			//SPAWN the WeaponBlueprint returning the spawned Actor to a variable called weapon of type AWeaponBase*
			AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnParams);
			//CALL Server_HoldWeapon() passing in weapon
			Server_HoldWeapon(Weapon);
		}
		//ENDIF

		//CALL UGameplayStatics::GetAllActorsWithTag() passing in GetWorld(), FName("PatrolPoint"), PatrolPoints
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PatrolPoint"), PatrolPoints);
		
		//IF PatrolPoints.Num() GREATER THAN 0
		if (PatrolPoints.Num() > 0)
		{
			//SET bPatrol to true
			bPatrol = true;
			//CALL MoveToNextPatrolPoint()
			MoveToNextPatrolPoint();
		}
		//ENDIF
	}
	//ENDIF


}

//TODO: WEEK11
void AAICharacterBase::OnPawnSeen(APawn* SeenPawn)
{
	//IF SeenPawn is nullptr
	if (!SeenPawn)
	{
		//return
		return;
	}
	//ENDIF

	//IF Cast<ACharacterBase>(SeenPawn)->GetHealth() NOT EQUAL TO nullptr AND Cast<ACharacterBase>(SeenPawn)->GetHealth()->IsDead()
   	if (Cast<ACharacterBase>(SeenPawn)->GetHealth() && Cast<ACharacterBase>(SeenPawn)->GetHealth()->IsDead())
    {
   		//IF GuardState IS EAIState::Alerted AND bPatrol IS false
		if (GuardState == EAIState::Alerted && !bPatrol)
			//CALL ResetPatrol()
			ResetPatrol();
   		//ENDIF

   		//RETURN
		return;
    }
	//ENDIF

	//SET LastSeenTime to the return of GetWorld()->GetTimeSeconds()
	LastSeenTime = GetWorld()->GetTimeSeconds();
	//SET bSensedTarget to true
	bSensedTarget = true;
	//SET TargetActor to SeenPawn
	TargetActor = SeenPawn;
	//CALL DrawDebugSphere() passing in GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12.f, FColor::Red, false, 10.f);
	
	

	//CALL SetGuardState() passing in EAIState::Alerted
	SetGuardState(EAIState::Alerted);

	//IF CurrentWeapon NOT EQUAL to nullptr
	if (CurrentWeapon)
		//CALL PullTrigger() on CurrentWeapon
		CurrentWeapon->PullTrigger();
	//ENDIF

	/*Stop Movement if Patrolling*/
	//DECLARE a variable called pController of type AController* and SET it to the return of GetController()
	AController* pController = GetController();
	//IF pCOntroller NOT EQUAL to nullptr
	if (pController)
	{
		
		//CALL StopMovement() on pController
		pController->StopMovement();
		//CALL GetWorldTimerManager().ClearTimer() passing in TimerHandle_ResetOrientation
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
		//CALL GetWorldTimerManager().SetTimer() passing in TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetPatrol, 3.0f
		GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetPatrol, 3.0f);
	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	//IF GuardState IS EAIState::Alerted
	if (GuardState == EAIState::Alerted)
	{
		//RETURN
		return;
	}
	//ENDIF

	//SET bSensedTarget to true
	bSensedTarget = true;
	//SET LastHeardTime to the return value of GetWorld()->GetTimeSeconds()
	LastHeardTime = GetWorld()->GetTimeSeconds();
	//CALL DrawDebugSphere() passing in GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f
	DrawDebugSphere(GetWorld(), Location, 32.f, 12.f, FColor::Green, false, 10.f);


	//DECLARE a variable called Direction of type FVector and Assign it to Location - GetActorLocation()
	FVector Direction = Location - GetActorLocation();
	//CALL Normalize() on Direction
	Direction.Normalize();
	//DECLARE a variable called NewLookAt of type FRotator and assign it the return value of FRotationMatrix::MakeFromX(Direction).Rotator()
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	//SET NewLookAt.Pitch to 0.0f
	NewLookAt.Pitch = 0.f;
	//SET NewLookAt.Roll to 0.0f
	NewLookAt.Roll = 0.f;
	//CALL SetActorRotation() passing in NewLookAt
	SetActorRotation(NewLookAt);

	//CALL GetWorldTimerManager().ClearTimer() passing in TimerHandle_ResetOrientation
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	//CALL GetWorldTimerManager().SetTimer() passing in TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetOrientation, 3.0f
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetOrientation, 3.f);

	//CALL SetGuardState() passing in EAIState::Suspicious
	SetGuardState(EAIState::Suspicious);

	//Stop Movement if Patrolling
	//DECLARE a variable called pController of type AController* and assign it to the return value of GetController()
	AController* pController = GetController();
	//IF pController NOT EQUAL to nullptr
	if (pController)
	{
		//CALL StopMovement() on pController
		pController->StopMovement();
	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::ResetOrientation()
{
	//IF GuardState IS EQUAL to EAIState::Alerted
	if (GuardState == EAIState::Alerted)
	{
		//RETURN
		return;
	}
	//ENDIF
	//CALL SetActorRotation() passing in OriginalRotator
	SetActorRotation(OriginalRotator);
	//CALL ResetPatrol()
	ResetPatrol();
}

//TODO: WEEK11
void AAICharacterBase::ResetPatrol()
{
	//SET TargetActor to nullptr
	TargetActor = nullptr;
	//CALL SetGuardState() passing in EAIState::Patrol
	SetGuardState(EAIState::Patrol);
	//IF CurrentWeapon NOT EQUAL to nullptr
	if (CurrentWeapon)
		//CALL ReleaseTrigger() on CurrentWeapon
		CurrentWeapon->ReleaseTrigger();
	//ENDIF

	//Stop Investigating ... if we are a patrolling pawn. Pick a new Patrol point to move to
	//IF bPatrol IS true
	if (bPatrol)
	{
		//CALL MoveToNextPatrolPoint()
		MoveToNextPatrolPoint();
	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::OnRep_GuardState()
{
	//Blueprint Event
	OnStateChanged(GuardState);
}

//TODO: WEEK11
void AAICharacterBase::SetGuardState(EAIState NewState)
{
	//IF GuardState IS EQUAL to NewState
	if (GuardState == NewState)
	{
		//RETURN
		return;
	}
	//ENDIF

	//SET GuardState to NewState
	GuardState = NewState;
	//CALL OnRep_GuardState() //USELESS
	OnRep_GuardState();

}
//TODO: WEEK11
void AAICharacterBase::MoveToNextPatrolPoint()
{

	//IF PatrolPoints.Num() IS EQUAL to 0
	if (PatrolPoints.Num() == 0)
		//RETURN
		return;
	//ENDIF

	//DECLARE a variable called Index of type int and SET it to 0
	int Index = 0;
	//IF CurrentPatrolPoint IS EQUAL to nullptr
	if (CurrentPatrolPoint == nullptr)
	{
		//SET CurrentPatrolPoint to element 0 in the PatrolPoints TArray
		CurrentPatrolPoint = PatrolPoints[0];
	}
	//ELSE
	else
	{
		//IF PatrolPoints.Find(CurrentPatrolPoint, Index)
		if (PatrolPoints.Find(CurrentPatrolPoint, Index))
		{
			//INCREMENT Index by 1
			Index++;
		}
		//ENDIF
	}
	//ENDIF

	//IF Index LESS THAN PatrolPoints.Num()
	if (Index < PatrolPoints.Num())
	{
		//SET CurrentPatrolPoint to the element at Index in the PatrolPoints TArray
		CurrentPatrolPoint = PatrolPoints[Index];
	}
	//ELSE
	else
	{
		//SET CurrentPatrolPoint to element 0 in the PatrolPoints TArray
		CurrentPatrolPoint = PatrolPoints[0];
	}
	//ENDIF

	//CALL UAIBlueprintHelperLibrary::SimpleMoveToActor() passing in GetController(), CurrentPatrolPoint
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

//TODO: WEEK11
void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*Patrol Goal Checks*/
	//IF CurrentPatrolPoint NOT EQUAL to nullptr
	
	{
		//DECLARE a variable called nearPatrol of type bool and SET it to the return value of  GetActorLocation().Equals(CurrentPatrolPoint->GetActorLocation(), 130)
	
		//IF nearPatrol IS true
	
		{
			//CALL MoveToNextPatrolPoint()
	
		}
		//ENDIF
	}
	//ENDIF


	/*Check Whether We still Sense the target*/
	if (bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut
		&& (GetWorld()->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{

			bSensedTarget = false;
			ResetPatrol();
	}

	//IF TargetActor NOT EQUAL to nullptr

	{
		//DECLARE a variable called MyLoc of type FVector and assign it to the return value of GetActorLocation()

		//DECLARE a variable called TargetLoc of type FVector and assign it to the return value of TargetActor->GetActorLocation()

		//DECLARE a variable called Dir of type FVector and Assign it to TargetLoc - MyLoc

		//CALL Normalize() on Dir

		//CALL SetActorRotation() passing in FMath::Lerp(GetActorRotation(), Dir.Rotation(), 1)

	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::Reset()
{
	//IF LocalRole is Authority
	if (GetLocalRole() == ROLE_Authority)
	{
		//UNCOMMENT the code below
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		   
		GetHealth()->MaximumHealth = 100;
		GetHealth()->CurrentHealth = 100;
					

		isActive = true;
		bPatrol = true;
		GuardState = EAIState::Patrol;
		MoveToNextPatrolPoint();

	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::OnDeath(AActor* KilledBy)
{
	//IF LocalRole IS Authority
	if (GetLocalRole() == ROLE_Authority)
	{

		//CREATE a TImer Using a lambda expression
		//UNCOMMENT THE LINES BELOW
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
			{
				// callback;
				isActive = false;
				spawner->ReleaseEnemy(this);

			});

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 3.0f, false);
				
	}
	//ENDIF
}

//TODO: WEEK11
void AAICharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//CALL DOREPLIFETIME() passing in AAICharacterBase, GuardState
	DOREPLIFETIME(AAICharacterBase, GuardState);

}

