// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "CharacterBase.h"
#include "CharacterBaseAnimation.h"

#include <GameFramework/PlayerController.h>
#include "Gameplay/Health/HealthComponent.h"
#include "Weapons/WeaponBase.h"
#include "Items/ItemBase.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryComponent.h"

ACharacterBase::ACharacterBase() :
	Health(nullptr),
	SkeletalMesh(nullptr),
	AnimationInstance(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set default movement speed.
	MaxWalkSpeed = 140.0f;
	MaxJogSpeed = 280.0f;

	GetCharacterMovement()->MaxWalkSpeed = MaxJogSpeed;

	// Do not update the controller's rotation yaw.
	// Has to be disabled for "Orient Rotation to Movement" to work.
	bUseControllerRotationYaw = false;

	bReplicates = true;
    SetReplicatingMovement(true);
	// Rotate the character during movement by default.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->SetNetAddressable();
	// Character interaction values.
	MaxPickUpDistance = 200.0f;

	// Character weapon handling values.
	CurrentWeapon = nullptr;

	bIsAiming = false;
	bIsFiring = false;

	SetCanBeDamaged(true);

	//TODO: WEEK11
	//Load the decal material
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorDecalMaterial = DecalMaterialAsset.Object;
	}
	//Create the Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Comp"));
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Retrieve the health component.
	Health = FindComponentByClass<UHealthComponent>();
	//check(Health != nullptr && "Character does not have a health component!");

	// Subscribe to health component's events.
	if(Health != nullptr)
	{
		Health->OnDeath.AddDynamic(this, &ACharacterBase::OnDeath);
	}
	// Retrieve the skeletal mesh component.
	SkeletalMesh = GetMesh();
	if (SkeletalMesh != nullptr)
	{
		// Retrieve the animation instance.
		AnimationInstance = Cast<UCharacterBaseAnimation>(SkeletalMesh->GetAnimInstance());
		//check(AnimationInstance != nullptr && "Character does not have an animation instance!");
	}


}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//TODO: WEEK11
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		//IF PC->IsLocalPlayerController()
		
		{
			//CREATE the CursorToWorld, this time using NewObject i.e NewObject<UDecalComponent>(this, UDecalComponent::StaticClass()). Assign the return value to CursorToWorld
		
			//CALL RegisterComponent() on CursorToWorld
		
			//CALL AttachToComponent() on CursorToWorld passing in RootComponent, FAttachmentTransformRules::KeepRelativeTransform
		

			//IF CursorDecalMaterial != nullptr
		
			{
				//CALL SetDecalMaterial() on CursorToWorld passing in CursorDecalMaterial
		
			}
			//ENDIF

			//SET CursorToWorld->DecalSize to FVector(16.0f, 32.0f, 32.0f)
		
			//CALL SetRelativeRotation() on CursorToWorld passing in FRotator(90.0f, 0.0f, 0.0f).Quaternion()
		
		}
		//ENDIF
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: WEEK11
	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			//IF PC->IsLocalPlayerController()
			
			{
				//DECLARE a variable called TraceHitResult of type FHitResult
			
				//CALL GetHitResultUnderCursor() on PC passing in ECC_Visibility, true, TraceHitResult
			
				//DECLARE a variable called CursorFV of type FVector and assign it to TraceHitResult.ImpactNormal
			
				//DECLARE a variable called CursorR of type FRotator and assign it to CursorFV.Rotation()
			
				//CALL SetWorldLocation() on CursorToWorld passing in TraceHitResult.Location
			
				//CALL SetWorldRotation() on CursorToWorld passing in CursorR
			

				//IF LastSelectedItem
			
					//CALL ShowSelect() on LastSelectedItem passing in false
			
				//ENDIF

				//IF TraceHitResult.GetActor()
			
				{
					//DECLARE a variable called Item of type AItemBase* and assign it to Cast<AItemBase>(TraceHitResult.GetActor())
			

					//IF Item
			
						//CALL ShowSelect() on Item passing in true (This shows the Highlight)
			
					//ENDIF

					//SET LastSelectedItem to Item
			
				}
				//ENDIF
				
			}
			//ENDIF

		}
	}
	
	// Handle movement orientation and speed.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = MaxJogSpeed;

	if(bHasWeapon && (bIsAiming || bIsFiring))
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;

		if(bIsAiming)
		{
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		}

		if(bIsFiring)
		{
			GetCharacterMovement()->MaxWalkSpeed *= 0.8;
		}
	}

	// Rotate the character towards the aiming point.
	if(bHasWeapon && (bIsAiming || bIsFiring))
	{
		auto PlayerController = Cast<APlayerController>(GetController());

		if(PlayerController)
		{
			// Cast cursor trace to world.
			FVector MouseLocation;
			FVector MouseDirection;

			bool Success = PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

			// Rotate the character towards the cursor.
			if(Success)
			{
				// Cast the cursor onto a plane.
				FVector LineBegin = MouseLocation;
				FVector LineEnd = MouseLocation + MouseDirection * 10000.0f;

				FVector PlaneOrigin = CurrentWeapon->GetMuzzleLocation();
				FVector PlaneNormal = FVector(0.0f, 0.0f, 1.0f);

				FVector LookLocation = FMath::LinePlaneIntersection(LineBegin, LineEnd, PlaneOrigin, PlaneNormal);

				// Rotate the cursor toward the intersection of the cursor and the plane.
				FRotator LookRotation = (LookLocation - GetActorLocation()).Rotation();

				LookRotation.Pitch = 0.0f;
				LookRotation.Roll = 0.0f;
				FRotator rot = FMath::RInterpTo(GetActorRotation(), LookRotation, DeltaTime, 10.0f);

				//TODO: 19 NET CODE 2 SHOOTING ChangeFacing(rot.Vector())
				ChangeFacing(rot.Vector()); 
			}
		}
	}

	// Set animation weapon parameters.
	AnimationInstance->bHasWeapon = bHasWeapon;
	AnimationInstance->bIsAiming = bIsAiming;

	// Set animation movement parameters.
	float CurrentSpeed = GetVelocity().Size();
	bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround();

	AnimationInstance->bIsMoving = bIsMoving;
	AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

	// Set animation strafing rotation paremeter.
	FVector MovementDirection = GetLastMovementInputVector();
	FVector CharacterDirection = GetActorForwardVector();

	if(!MovementDirection.IsNearlyZero())
	{
		float StrafingRotation = FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X);

		if(FMath::Abs(StrafingRotation) > PI)
		{
			StrafingRotation = StrafingRotation > 0 ? StrafingRotation - PI * 2.0f : StrafingRotation + PI * 2.0f;
		}

		StrafingRotation = FMath::RadiansToDegrees(StrafingRotation);

		AnimationInstance->StrafingRotation = StrafingRotation;
	}
}

//TODO: 18 NET CODE 2 SHOOTING
void ACharacterBase::ChangeFacing(FVector TargetVector)
{
	// We set the characters facing to match the facing of the vector
	SetActorRotation(TargetVector.Rotation());

	// We set our current facing to the direction the actor is facing
	CurrentFacing = TargetVector;
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ChangeFacing(CurrentFacing);
	}
}
//TODO: 20 NET CODE 2 SHOOTING
void ACharacterBase::Server_ChangeFacing_Implementation(FVector TargetVector)
{
	ChangeFacing(TargetVector);
}
//TODO: 21 NET CODE 2 SHOOTING
bool ACharacterBase::Server_ChangeFacing_Validate(FVector TargetVector)
{
	return true;
}
//TODO: 22 NET CODE 2 SHOOTING
void ACharacterBase::Server_SetIsAiming_Implementation(bool val)
{
	bIsAiming = val;
}
//TODO: 23 NET CODE 2 SHOOTING
bool ACharacterBase::Server_SetIsAiming_Validate(bool val)
{
	return true;
}
//TODO: 24 NET CODE 2 SHOOTING
void ACharacterBase::Server_SetIsFiring_Implementation(bool val)
{
	bIsFiring = val;
}
//TODO: 25 NET CODE 2 SHOOTING
bool ACharacterBase::Server_SetIsFiring_Validate(bool val)
{
	return true;
}
//TODO: 26 NET CODE 2 SHOOTING
void ACharacterBase::SetHasWeapon(bool val)
{
	bHasWeapon = val;
}
//TODO: 27 NET CODE 2 SHOOTING
//TODO: WEEK8 - Is there other Information we can pass here
void ACharacterBase::ApplyDamage(AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("ACharacterBase:: DamageCaused"));

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Damage Caused By: " + DamageCauser->GetActorLabel());
	FDamageEvent DamageEvent(UDamageType::StaticClass());
	TakeDamage(10.0f, DamageEvent, nullptr, DamageCauser);
}

void ACharacterBase::Move(FVector Direction, float Scale)
{
	AddMovementInput(Direction, Scale);
}

void ACharacterBase::Fire(bool Toggle)
{
	if(CurrentWeapon)
	{
		if(Toggle)
		{
			CurrentWeapon->PullTrigger();
		}
		else
		{
			CurrentWeapon->ReleaseTrigger();
		}
		//TODO: 28 NET CODE 2 SHOOTING Server_SetIsFiring(Toggle); IN ACharacterBase::Fire()
		Server_SetIsFiring(Toggle);
	}
}

void ACharacterBase::Aim(bool Toggle)
{
	//TODO: 29 NET CODE 2 SHOOTING Server_SetIsAiming(Toggle) IN Aim()
	Server_SetIsAiming(Toggle);
}

void ACharacterBase::Interact(AActor* Actor)
{

	UE_LOG(LogClass, Log, TEXT("%s pawn begin play: %s .. ROLE on machine: %s / REMOTE ROLE: %s  "),
		(IsLocallyControlled() ? TEXT("LOCAL") : TEXT("proxy")),
		*GetName(),
		*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()),
		*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()));

	//ACharacterBase* Charac = Cast<ACharacterBase>(Actor);
	//if (Charac)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("ACharacterBase:: Picked CharacterBase"));
	//	Charac->SetOwner(GetController()->GetOwner());
	//	GetController()->Possess(Charac);
	//}
	// Drop the current weapon.
	//TODO: NET CODE 1 NET PICKUP
	//Server_DropWeapon();
	// Check if the actor is an item.
	AItemBase* Item = Cast<AItemBase>(Actor);

	if(Item == nullptr)
		return;

	// Check distance from the object.
	float Distance = Item->GetDistanceTo(this);

	if(Distance > MaxPickUpDistance)
		return;

	if(Item->IsA<AWeaponBase>())
	{
		// Hold the item if it's a weapon.
		AWeaponBase* Weapon = Cast<AWeaponBase>(Item);

		if(Weapon != nullptr)
		{		
			//TODO: WEEK11
			//CALL Server_Pickup() passing in Weapon
			
			//CALL Server_HoldWeapon() passing in Weapon
			
		}
	}


}
//TODO: NET CODE 1 NET PICKUP
bool ACharacterBase::Server_HoldWeapon_Validate(AWeaponBase* Weapon)
{
	return true;
}
//TODO: WEEK11
void ACharacterBase::Server_HoldWeapon_Implementation(AWeaponBase* Weapon)
{
	check(Weapon != nullptr && "Passed a null weapon!");

	if (Weapon)
	{
		//NET CODE 3
		//IF the ROle is Role_Authority
		if (GetLocalRole() == ROLE_Authority)
		{
			//IF CurrentWeapon
			
			{
				//CALL ReleaseTrigger() on CurrentWeapon
			
				//CALL NMC_Disable() on CurrentWeapon
			
			}
			//ENDIF
			
			//SET/ASSIGN CurrentWeapon to Weapon
			

			//CALL AttachCurrentWeapon()
			

			//CALL NMC_Enable() on CurrentWeapon
			
		}
		//ENDIF
	}
	
	SetHasWeapon(CurrentWeapon != nullptr);
	
}

void ACharacterBase::AttachCurrentWeapon()
{

	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->Attach(this);
		CurrentWeapon->OnWeaponFired.Clear();
		// Subscribe to weapon's events.
		CurrentWeapon->OnWeaponFired.AddDynamic(this, &ACharacterBase::OnWeaponFired);
	}
	
}


//TODO: WEEK11
void ACharacterBase::Server_DropWeapon_Implementation()
{
	//IF CurrentWeapon NOT nullptr
	
	{
		//CALL RemoveInventoryItem() on InventoryComponent passing in CurrentWeapon
		

		//CALL NoWeapon() passing in true
		

		// Reset weapon states states.
		//SET bIsFiring to false
		
		//SET bIsAiming to false
		
		
	}
	//ENDIF
	
	SetHasWeapon(CurrentWeapon != nullptr);
}

bool ACharacterBase::Server_DropWeapon_Validate()
{
	return true;
}

//TODO: 30 NET CODE 2 SHOOTING NMC_PlayWeaponFiringAnimation();
void ACharacterBase::OnWeaponFired()
{
	// Play recoil animation depending on the stance.
	NMC_PlayWeaponFiringAnimation();
}
//TODO: 31 NET CODE 2 SHOOTING
void ACharacterBase::NMC_PlayWeaponFiringAnimation_Implementation()
{
	if (bIsAiming)
	{
		AnimationInstance->Montage_Play(FireAimAnimation);
	}
	else
	{
		AnimationInstance->Montage_Play(FireHipAnimation);
	}
}
//TODO: 32 NET CODE 2 SHOOTING
void ACharacterBase::NMC_OnClientDeath_Implementation()
{
	// Disable character's capsule collision.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Allow character's ragdoll to be pushed around.
	/*
	SkeletalMesh->SetCollisionProfileName(TEXT("BlockAll"));
	SkeletalMesh->CanCharacterStepUpOn = ECB_No;
	SkeletalMesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	*/
	UE_LOG(LogTemp, Warning, TEXT("DEAD"));
	// Simulate character's ragdoll.
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetSimulatePhysics(true);
}

//Will be fired on Server
void ACharacterBase::OnDeath(AActor* KilledBy)
{

	// Stop ticking while dead.
	PrimaryActorTick.bCanEverTick = false;

	// Replicated Drop held weapon. 
	Server_DropWeapon();

	//TODO: 34 NET CODE 2 SHOOTING NMC_OnClientDeath(); 
	NMC_OnClientDeath();

	//TODO: WEEK8
	//IF IsPlayerControlled()
	if(IsPlayerControlled())
		//CALL DetachFromControllerPendingDestroy()
		DetachFromControllerPendingDestroy();
	//ENDIF
	
}


AWeaponBase* ACharacterBase::GetCurrentWeapon()
{
	return CurrentWeapon;
}

class USkeletalMeshComponent* ACharacterBase::GetSkeletalMesh()
{
	return SkeletalMesh;
}

//TODO: WEEK11
void ACharacterBase::Server_NextInventoryItem()
{
	//IF CurrentWeapon
	
		//CALL ReleaseTrigger() on CurrentWeapon
	
	//ENDIF

	//CALL NextInventoryItem() on InventoryComponent
	
	//DECLARE a variable called Weapon of type AWeaponBase* and assign it to  Cast<AWeaponBase>(InventoryComponent->GetCurrentInventory())
	
	//IF Weapon
	
	{
		//CALL Server_HoldWeapon() passing in Weapon
	
	}
	//ELSE
	
	{
		//CALL NoWeapon() passing in false
	
	}
	//ENDIF
}
//bool ACharacterBase::Server_NextInventoryItem_Validate()
//{
//	return true;
//}

//TODO: WEEK11
void ACharacterBase::Server_PrevInventoryItem()
{
	//IF CurrentWeapon

		//CALL ReleaseTrigger() on CurrentWeapon

	//ENDIF

	//CALL PreviousInventoryItem() on InventoryComponent


	//DECLARE a variable called Weapon of type AWeaponBase* and assign it to  Cast<AWeaponBase>(InventoryComponent->GetCurrentInventory())

	//IF Weapon

	{
		//CALL Server_HoldWeapon() passing in Weapon

	}
	//ELSE
	
	{
		//CALL NoWeapon() passing in false

	}
	//ENDIF
}
//bool ACharacterBase::Server_PrevInventoryItem_Validate()
//{
//	return true;
//}

//TODO: WEEK11
void ACharacterBase::Server_Pickup(class AItemBase* PickedItem)
{
	//CALL AddToInventory() on InventoryComponent passing in PickedItem
	
}

//bool ACharacterBase::Server_Pickup_Validate(class AItemBase* PickedItem)
//{
//	return true;
//}

//TODO: WEEK11
void ACharacterBase::NoWeapon(bool discard)
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->OnWeaponFired.RemoveDynamic(this, &ACharacterBase::OnWeaponFired);
	CurrentWeapon->OnWeaponFired.Clear();
	// Detach weapon from the character.
	if (discard)
		CurrentWeapon->Detach();
	else
	{
		CurrentWeapon->ReleaseTrigger();
		CurrentWeapon->NMC_Disable();
	}

	SetHasWeapon(CurrentWeapon != nullptr);
}

//TODO: WEEK11
void ACharacterBase::InteractDropWeapon()
{
	//CALL Server_DropWeapon()
	
}
void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, CurrentWeapon);
	DOREPLIFETIME(ACharacterBase, CurrentFacing);
	DOREPLIFETIME(ACharacterBase, bIsAiming);
	DOREPLIFETIME(ACharacterBase, bIsFiring);
	DOREPLIFETIME(ACharacterBase, bHasWeapon);

}