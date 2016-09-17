// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseCharacter.h"
#include "ControllableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ALIZARINGAME_API AControllableCharacter : public ABaseCharacter
{
	GENERATED_BODY()

private:
	FVector movementVector;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		float moveSpeed;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	
};
