// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageableInterface.generated.h"


UINTERFACE(BlueprintType)
class ALIZARINGAME_API UDamageableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class ALIZARINGAME_API IDamageableInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
		void TickDamage(float deltaSeconds);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
		void CalculateDamage(float damage);
};
