// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EInventoryDirection : uint8 {

	NEXT = 0  UMETA(DisplayName = "NEXT"),
	PREV = 1  UMETA(DisplayName = "PREV")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Region_Private

#pragma endregion Region_Private

#pragma region Region_Public
public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddToInventory(class AItemBase* pickup);
	class AItemBase* RemoveCurrentInventoryItem();

	class AItemBase* RemoveInventoryItem(class AItemBase* pickup);

	int GetInventoryCount();

	/**
	* Getter for the currently equipped item.
	* @return Current item
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		class AItemBase* GetCurrentInventory() const;

	void ResetCurrentInventory();

#pragma endregion Region_Public

#pragma region Region_Protected
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class ACharacterBase* CharacterOwner;

	void SelectInventory(EInventoryDirection dir);

#pragma endregion Region_Protected


#pragma region Networking

public:
    UPROPERTY(VisibleAnywhere, Replicated)
        TArray< class AItemBase* > Inventory;
    /** 
    * Cycle through the inventory array and equip the next item.
    */
   // UFUNCTION(BlueprintCallable, Category = "Inventory", Server, Reliable, WithValidation)
        virtual void NextInventoryItem();

    /**
    * Cycle through the inventory array and equip the previous item.
    */
   // UFUNCTION(BlueprintCallable, Category = "Inventory", Server, Reliable, WithValidation)
        virtual void PreviousInventoryItem();
protected:

	//int32 Index = -1;
    /** Item that is currently equipped */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Replicated)
        class AItemBase* CurrentInventory;

#pragma endregion Networking functions
};
