// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "ProjectileBase.h"

#include "AI/AICharacterBase.h"
#include "Characters/CharacterBase.h"
AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default projectile parameters.
	Damage = 10.0f;
	Speed = 1200.0f;
	LifeTime = 1.0f;

	//TODO: 36 NET CODE 2 SHOOTING
	SetReplicates(true);
	SetReplicateMovement(true);

}

void AProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//TODO: 37 NET CODE 2 SHOOTING
	// Subscribe to actor's hit event.
	if(GetLocalRole() == ROLE_Authority)
		AActor::OnActorHit.AddDynamic(this, &AProjectileBase::OnActorHit);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	//TODO: 38 NET CODE 2 SHOOTING
	// Destroy the projectile after some time.
	if (GetLocalRole() == ROLE_Authority)
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectileBase::DestroySelf, LifeTime, false);
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the projectile in a forward direction.
	FVector Location = GetActorLocation();
	Location += GetActorForwardVector() * Speed * DeltaTime;
	SetActorLocation(Location, true);
}

//TODO: WEEK11
void AProjectileBase::OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit)
{

	// Cause damage to the hit actor.
	if(Other != nullptr)
	{
	
		if (GetLocalRole() == ROLE_Authority)
		{
			ACharacterBase* CharacterB = Cast<ACharacterBase>(Other);

			/*Prevent AI destroying each other*/
			//TODO: WEEK11
			if(Other->IsA<AAICharacterBase>() && this->GetInstigator()->IsA<AAICharacterBase>())
				return;

			
			if (CharacterB)
			{
				//TODO: 39 NET CODE 2 SHOOTING ????
				if (CharacterB->GetLocalRole() == ROLE_Authority)
				{
					/*Pass In the Instigator to ApplyDamage on the Character*/
					//TODO: WEEK11
					CharacterB->ApplyDamage(this->GetInstigator());
				}
				
			}
		}
		
	}

	// Destroy self.

	//TODO: 40 NET CODE 2 SHOOTING
	if (GetLocalRole() == ROLE_Authority) //Checks whether we are the server
	{
		Destroy();
	}
}

void AProjectileBase::DestroySelf()
{
	//TODO: 41 NET CODE 2 SHOOTING
	// Destroy self.
	if (GetLocalRole() == ROLE_Authority)
		Destroy();
}
