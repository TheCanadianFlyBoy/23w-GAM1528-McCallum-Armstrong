// Fill out your copyright notice in the Description page of Project Settings.


//Self
#include "Projectile.h"
//Components
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpeed = 3000.f;

	//Create collision sphere
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileSphere"));
	SphereComponent->SetSphereRadius(30.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionProfileName("BlockAllDynamic");
	SetRootComponent(SphereComponent);
	//Create Sphere Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetupAttachment(RootComponent);

	//Subscribe to collision
	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnCollision);

	//Movement
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
	MovementComponent->InitialSpeed = ProjectileSpeed;
	MovementComponent->ProjectileGravityScale = 0.f;
	

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != GetOwner() && !Cast<AProjectile>(OtherActor)) {
		//Audio
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, GetActorLocation());
		//Self destroy
		this->Destroy();
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

