// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->SetWorldScale3D(FVector(0.45f));
	RootComponent = MeshComponent;
	//Movement
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->InitialSpeed = 800.f;
	//Radial Force
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->Radius = DamageRadius;
	RadialForceComponent->DestructibleDamage = BaseDamage;
	RadialForceComponent->ImpulseStrength = 200000.f;
	RadialForceComponent->bAutoActivate = false;


	//Collisions
	if (GetLocalRole() == ROLE_Authority)
		MeshComponent->OnComponentHit.AddDynamic(this, &AGrenade::OnCollision);

	//Replicate
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrenade::Explode_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, GetActorLocation());

	RadialForceComponent->FireImpulse();


	this->Destroy();
}

void AGrenade::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != GetOwner()) {
		//Freeze in place
		MovementComponent->Velocity = FVector(0.f);
		MovementComponent->ProjectileGravityScale = 0.f;
		MeshComponent->SetCollisionProfileName("OverlapAll");

		bArmed = true;
		//TODO: trigger countdown
		if (!GetWorld()->GetTimerManager().IsTimerActive(ExplosiveTimer))
		{
			//BEIGE
			GetWorld()->GetTimerManager().SetTimer(ExplosiveTimer, this, &AGrenade::Explode, 5.f, false);
		}
	}
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bArmed)
	{
		MeshComponent->SetRelativeScale3D(FVector(MeshComponent->GetRelativeScale3D() * (0.999f)));
		MeshComponent->AddLocalRotation(FRotator(10.f, 30.f, 80.f) * DeltaTime * RotationSpeed);
		RotationSpeed += DeltaTime * 20.f;
	}

}

