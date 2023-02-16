// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBombActor.h"
//Components
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "FPSProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "BombDamageType.h"


// Sets default values
AFPSBombActor::AFPSBombActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create box
	BombBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BombBox"));
	BombBox->SetCollisionProfileName("BlockAll");
	BombBox->SetSimulatePhysics(true);
	BombBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BombBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	RootComponent = BombBox;
	//Subsnirb
	BombBox->OnComponentHit.AddDynamic(this, &AFPSBombActor::OnHit);
	//Create mesh
	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(RootComponent);
	BombMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Radial Force
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->Radius = 1000.f;
	RadialForceComponent->ImpulseStrength = 200000.f;
	RadialForceComponent->SetAutoActivate(false);
	//Particles
	

}

// Called when the game starts or when spawned
void AFPSBombActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSBombActor::Explode()
{
	RadialForceComponent->FireImpulse();

	TArray<AActor*> Ignored;

	bool beige = UGameplayStatics::ApplyRadialDamage(GetWorld(), 10.f, BombBox->GetComponentLocation(), RadialForceComponent->Radius, DamageType, Ignored, this, GetInstigatorController(), false, ECC_WorldDynamic);
	DrawDebugSphere(GetWorld(), BombBox->GetComponentLocation(), RadialForceComponent->Radius, 64, FColor::Red, true);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionTemplate, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	GetWorld()->GetTimerManager().SetTimer(FuseTimer, this, &AFPSBombActor::DestroyBomb, 1.f, false);
}

void AFPSBombActor::DestroyBomb()
{
	if (BombBox->GetAttachParent())
	{
		BombBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	Destroy();
}

void AFPSBombActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Cast<APawn>(OtherActor))
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (!TimerManager.IsTimerActive(FuseTimer))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "TIMER");
			TimerManager.SetTimer(FuseTimer, this, &AFPSBombActor::Explode, ExplodeDelay, false);
		}
	}
}

// Called every frame
void AFPSBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSBombActor::Hold(USkeletalMeshComponent* HoldingComponent)
{
	if (HoldingComponent)
	{
		BombBox->SetSimulatePhysics(false);

		BombBox->AttachToComponent(HoldingComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), "Spine");

	}
}

void AFPSBombActor::Throw(FVector Direction)
{
	if (BombBox->GetAttachParent())
	{
		BombBox->SetSimulatePhysics(true);
		BombBox->SetNotifyRigidBodyCollision(true);
		//Detach
		BombBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		//Throw
		float throwing_force = 100000.f;
		BombBox->AddImpulse(Direction * throwing_force);

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	}

}

