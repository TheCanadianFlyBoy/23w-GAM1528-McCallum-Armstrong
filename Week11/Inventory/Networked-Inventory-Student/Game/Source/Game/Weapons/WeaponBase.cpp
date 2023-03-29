// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "WeaponBase.h"

#include <Components/SkeletalMeshComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/ArrowComponent.h>
#include "Characters/CharacterBase.h"
#include "Net/UnrealNetwork.h"

AWeaponBase::AWeaponBase() :
	Primitive(nullptr),
	Muzzle(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set default variables.
	FireRate = 10.0f;

	MaximumAmmo = 30;
	CurrentAmmo = -1;

	//TODO: NET CODE 1 NET PICKUP
	SetReplicates(true);
	SetReplicateMovement(true);

}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get the root primitive component.
	Primitive = Cast<UPrimitiveComponent>(GetRootComponent());
	//check(Primitive != nullptr && "Actor is mising a primitive component!");

	// Get the muzzle arrow component.
	auto Components = GetComponents();
	for(auto Component : Components)
	{
		if(Component->GetFName() == "Muzzle")
		{
			Muzzle = Cast<UArrowComponent>(Component);
			break;
		}
	}

	//check(Muzzle != nullptr && "Actor is mising an arrow component for muzzle!");

	// Sanitize default values.
	MaximumAmmo = FMath::Max(1, MaximumAmmo);
	CurrentAmmo = FMath::Min(CurrentAmmo, MaximumAmmo);

	// Set current ammo to maximum ammo.
	if(CurrentAmmo < 0)
	{
		CurrentAmmo = MaximumAmmo;
	}
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::Attach(class ACharacterBase* Character)
{
	verify(Character != nullptr && "Weapon's attach function called with null character!");

	// Set owner of this weapon.
	SetOwner(Character);

	//TODO: NET CODE 1 NET PICKUP
	// Disable weapon's physics.
	if (GetLocalRole() == ROLE_Authority)
	{
		SetActorEnableCollision(false);
		Primitive->SetSimulatePhysics(false);
	}
	// Attach weapon to the character's mesh.
	AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
}

void AWeaponBase::Detach()
{
	// Stop firing when the weapon is detached.
	ReleaseTrigger();

	// Unset owner of the weapon.
	SetOwner(nullptr);

	//TODO: NET CODE 1 NET PICKUP
	// Re-enable weapon's physics.
	if (GetLocalRole() == ROLE_Authority)
	{
		SetActorEnableCollision(true);
		Primitive->SetSimulatePhysics(true);
	}
	// Detach weapon from the character's mesh.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AWeaponBase::PullTrigger()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	// Check current ammo value before attempting to firing.
	if(CurrentAmmo > 0)
	{
		// Start the firing timer and use the remaining time of the previous timer.
		float RemainingTime = FMath::Max(TimerManager.GetTimerRemaining(FireTimer), 0.0f);
		TimerManager.SetTimer(FireTimer, this, &AWeaponBase::Fire, 1.0f / FireRate, true, RemainingTime);
	}
}

void AWeaponBase::ReleaseTrigger()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	// Replace timer with one that will prevent the primary fire timer from triggering again too quickly.
	if(TimerManager.TimerExists(FireTimer))
	{
		float RemainingTime = TimerManager.GetTimerRemaining(FireTimer);
		TimerManager.SetTimer(FireTimer, this, &AWeaponBase::ClearFireTimer, RemainingTime, false);
	}
}

void AWeaponBase::Fire()
{
	//TODO: 42 NET CODE 2 SHOOTING
		Server_Fire();
}

//TODO: 43 NET CODE 2 SHOOTING
void AWeaponBase::Server_Fire_Implementation()
{
		// Check current ammo value before actually firing.
		if (CurrentAmmo > 0)
		{
			// Spawn a projectile.
			FActorSpawnParameters SpawnParams;
			
			/*Set the Instigator of the projectile to the Owner of this Weapon (Players Character or AI)*/
			//TODO: WEEK8 
			SpawnParams.Instigator = Cast<APawn>(GetOwner());
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
																										  
																										  
			FTransform Transform = Muzzle->GetComponentToWorld();
			GetWorld()->SpawnActor<AActor>(ProjectileType, Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParams);

			// Decrement the ammo count.
			CurrentAmmo -= 1;
		}
		// Broadcast a weapon fired event.
		OnWeaponFired.Broadcast();


}
//TODO: 44 NET CODE 2 //for anti cheat. returning false disconnects the client
bool AWeaponBase::Server_Fire_Validate()
{
	return true;
}
void AWeaponBase::ClearFireTimer()
{
	// Clear the timer after a delay set in ReleaseTrigger() function.
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

FVector AWeaponBase::GetMuzzleLocation() const
{
	return Muzzle->GetComponentToWorld().GetLocation();
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, FireRate);
	DOREPLIFETIME(AWeaponBase, MaximumAmmo);
	DOREPLIFETIME(AWeaponBase, CurrentAmmo);
}
