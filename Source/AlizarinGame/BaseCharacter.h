// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"

#include "BaseCharacter.generated.h"

UCLASS()
class ALIZARINGAME_API ABaseCharacter : public APawn 
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Properties related to health
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
		float maximumHealth = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
		float currentHealth = 100;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Health")
		float percentageHealth = 1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Health")
		bool isDead = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
		float healthRegeneration = 0; // health per second

	// Properties related to shields
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
		float maximumShield = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
		float currentShield = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Shield")
		float percentageShield = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
		float shieldRegeneration = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
		float shieldRechargeDelay = 3; // time before shield recharges

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Shield")
		float shieldRechargeDelayRemaining = 0;

	// Functions for taking damage (TO BE HOOKED UP TO INTERFACE)
	UFUNCTION(BlueprintCallable, Category = "Damage")
		virtual void CalculateDamage(float damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
		virtual void TickDamage(float deltaSeconds);

};
