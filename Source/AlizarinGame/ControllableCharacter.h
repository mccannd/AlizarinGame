// The class used for the player-controlled character
#pragma once

#include "BaseCharacter.h"
#include "BaseWeapon.h"
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

	// The player's currently equipped weapon
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equipment")
		TSubclassOf<ABaseWeapon> activeWeapon = NULL;

	// Enable movement Debug printing on screen?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
		bool moveDebug = false;

	// Enable mouse Debug printing on screen?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
		bool mouseDebug = false;

	// Enable equipment Debug printing on screen?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
		bool equipDebug = false;

	// Equip a weapon
	UFUNCTION(BlueprintCallable, Category = "Equipment")
		void EquipWeapon(TSubclassOf<ABaseWeapon> toEquip);

	/// player inputs

	// movement inputs
	UFUNCTION()
	void MoveVertical(float AxisValue);

	UFUNCTION()
	void MoveHorizontal(float AxisValue);

	// weapon inputs
	UFUNCTION()
		void PrimaryFireHold();

	UFUNCTION()
		void PrimaryFireRelease();
};
