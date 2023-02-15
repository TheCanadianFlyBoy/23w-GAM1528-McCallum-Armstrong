// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UseInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUseInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIMPLEDOOR_API IUseInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// classes using this interface must implement BlueprintNativeEvent
	/**A version of BlueprintNativeEvent that can be implemented in C++ or Blueprint. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Use();

	//classes using this interface may implement Use()
	/**A version of BlueprintImplementableEvent that can be implemented in Blueprint only. */
	/* 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Use();*/

	virtual void SomeFunc(); //Needs Implementaion in Cpp

	virtual void SomeFunc2() = 0;
};
