// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "UE5_Lab2Character.h"
#include "Components/TextRenderComponent.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickupZone"));
	SphereComponent->SetCollisionProfileName("OverlapAllDynamic");
	SphereComponent->SetSphereRadius(200.f);
	RootComponent = SphereComponent;

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->AddLocalOffset(FVector(0.f, 0.f, 200.f));
	TextComponent->SetText(FText::FromString("Gib Gremndas"));

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::RestoreGrenades);

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickup::RestoreGrenades(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AUE5_Lab2Character* Character = Cast<AUE5_Lab2Character>(OtherActor);
	if (Character)
	{
		Character->RefillGrenades();
	}
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

