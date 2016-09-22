// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class ALIZARINGAME_API UWeaponInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class ALIZARINGAME_API IWeaponInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// different strategies for firing weapon:
	// auto-attacking: holding causes repeated shots with interval
	// charge: holding builds charge, releasing fires weapon
	// semi-auto (possible): has slow auto, but release resets timer

	// called repeatedly as long as player holds down mouse
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
		void FireHold();

	// called when player releases mouse
	UFUNCTION(BluePrintCallable, BlueprintNativeEvent, Category = "Interaction")
		void FireRelease();
};
