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
	FVector xMovementVector;
	FVector yMovementVector;

	bool findMouseRotation(FRotator& rotation);
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		float moveSpeed = 1;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// player inputs
	UFUNCTION()
	void MoveVertical(float AxisValue);

	UFUNCTION()
	void MoveHorizontal(float AxisValue);

	
};
