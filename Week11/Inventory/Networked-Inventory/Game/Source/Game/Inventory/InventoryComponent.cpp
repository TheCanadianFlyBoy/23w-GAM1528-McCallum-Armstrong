// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
//#include "Items/ItemBase.h"
#include "Net/UnrealNetwork.h" //needed for DOREPLIFETIME
#include "Characters/CharacterBase.h"

// Sets default values for this component's properties

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


//TODO: WEEK11
// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

    CharacterOwner = Cast<ACharacterBase>(GetOwner());
	check(CharacterOwner != nullptr && "UInventoryComponent::BeginPlay - CharacterOwner is Null");

	//CALL SetIsReplicated() passing in true
	SetIsReplicated(true);
        
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::AddToInventory(class AItemBase* pickup)
{
		Inventory.Add(pickup);
		CurrentInventory = pickup;
}

class AItemBase* UInventoryComponent::RemoveCurrentInventoryItem()
{
         int32 Index = 0;
	
        if (CurrentInventory)
        {
		
		    if (Inventory.Find(CurrentInventory, Index))
		    {
			    Inventory.RemoveAt(Index, 1, true);
			
			    CurrentInventory->SetActorLocation(GetOwner()->GetActorLocation());

			    ResetCurrentInventory();
			    return CurrentInventory;
		    }
        }
	return nullptr;	
}


class AItemBase* UInventoryComponent::RemoveInventoryItem(class AItemBase* pickup)
{
		int32 Index = 0;

		if (pickup)
		{
			if (Inventory.Find(pickup, Index))
			{
				Inventory.RemoveAt(Index, 1, true);

                pickup->SetActorLocation(GetOwner()->GetActorLocation());

				ResetCurrentInventory();
				return CurrentInventory;
			}
		}

	return nullptr;
}

class AItemBase* UInventoryComponent::GetCurrentInventory() const
{
	return CurrentInventory;
}

void UInventoryComponent::ResetCurrentInventory()
{
	CurrentInventory = nullptr;
}

int UInventoryComponent::GetInventoryCount()
{
	return Inventory.Num();
}


void UInventoryComponent::NextInventoryItem()
{

	SelectInventory(EInventoryDirection::NEXT);
}


void UInventoryComponent::PreviousInventoryItem()
{
   
	SelectInventory(EInventoryDirection::PREV);
}

void UInventoryComponent::SelectInventory(EInventoryDirection dir)
{
	//No items means there is nothing to equip
	//IF Inventory Num IS 0
	if (Inventory.Num() == 0)
	{
		//RETURN
		return;
	}
	//ENDIF

	//Get the index of the current item and decrement it
	//DECLARE a variable of type APickupActor* called InventoryItemToEquip and SET it to nullptr
	
	AItemBase* InventoryItemToEquip = nullptr;
	//Use a Ternary Operator to determine the Index https://www.w3schools.com/cpp/cpp_conditions_shorthand.asp
	//UNCOMMENT the line below
	int32 Index = (dir == EInventoryDirection::NEXT ? 0 : Inventory.Num() - 1);
	//IF CurrentInventoryItem IS NOT nullptr
	if (CurrentInventory)
	{
		//CALL Find() on Inventory passing in CurrentInventoryItem, Index
		Inventory.Find(CurrentInventory, Index);

		//Use a Ternary Operator to determine the Index https://www.w3schools.com/cpp/cpp_conditions_shorthand.asp
		//UNCOMMENT one of the two lines below, they do the same thing
		//dir == EInventoryDirection::NEXT ? Index += 1 : Index -= 1;
		//OR
		Index += (dir == EInventoryDirection::NEXT ? 1 : -1);
	}
	//ENDIF

	/*Remove Two If's if you want null(Empty) Inventory Item*/
	if(dir == EInventoryDirection::NEXT && Index >= Inventory.Num())
		Index = 0;

	if (dir == EInventoryDirection::PREV && Index < 0)
		Index = Inventory.Num() - 1;

	//Set new item when index is inside array
	//IF Index IS GREATER THAN -1 AND Index LESS THAN Inventory.Num()
	if (Index < Inventory.Num() && Index > -1)
		//SET InventoryItemToEquip to Inventory[Index]
		InventoryItemToEquip = Inventory[Index];
	//ENDIF

	
	CurrentInventory = InventoryItemToEquip;
	if(CurrentInventory)
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Index=" + FString::FromInt(Index));
}

//TODO: WEEK11
void UInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*Replicate variable to Owning player only*/
	//CALL DOREPLIFETIME_CONDITION() passing in UInventoryComponent, CurrentInventory, COND_OwnerOnly
    DOREPLIFETIME_CONDITION(UInventoryComponent, CurrentInventory, COND_OwnerOnly);
}


