// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "HealthComponent.h"
#include "Util/HealDamageType.h"
#include "Net/UnrealNetwork.h"
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// Set default values.
	MaximumHealth = 100.0f;
	CurrentHealth = -1.0f;
	
	
}

void UHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);

	// Sanitize default values.
	MaximumHealth = FMath::Max(1.0f, MaximumHealth);
	CurrentHealth = FMath::Min(CurrentHealth, MaximumHealth);

	// Set current health to maximum health.
	if(CurrentHealth < 0.0f)
	{
		CurrentHealth = MaximumHealth;
	}
	GetOwner()->SetCanBeDamaged(true);
	// Subscribe to the delegate in the attached actor.
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Broadcast actor's death event if initial health is zero.
	if(CurrentHealth == 0.0f)
	{
		OnDeath.Broadcast(nullptr);
	}
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	/*UE_LOG(LogTemp, Warning, TEXT("UHealthComponent:: DamageCauser Is %s"), *DamageCauser->GetName());
	UE_LOG(LogTemp, Warning, TEXT("UHealthComponent:: Character Hit %s"), *DamagedActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("UHealthComponent:: Character Hit %f"), Damage);*/

	// Check if already dead.
	if(IsDead())
		return;

	/*If the DamageType is UHealDamageType then give health. UHealDamageType is sent from one of the PainCausing Volumes*/
	//TODO: WEEK8 
	const UHealDamageType* Heal = Cast<UHealDamageType>(DamageType);
	// Update the current health value.
	if(Heal)
		CurrentHealth = FMath::Clamp(CurrentHealth + Damage, 0.0f, MaximumHealth);
	else
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaximumHealth);

	// Handle current health value reaching zero.
	if(CurrentHealth <= 0.0f)
	{
		// Broadcast actor's death event.
		OnDeath.Broadcast(DamageCauser);
	}
}

bool UHealthComponent::IsDead() const
{
	return CurrentHealth <= 0.0f;
}

//TODO: 35 NET CODE 2 SHOOTING
void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}
