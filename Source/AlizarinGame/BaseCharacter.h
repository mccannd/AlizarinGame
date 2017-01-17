// Class concerning all characters, both player and enemy
// covers baseline health mechanics but not complex interactions
// most character behaviors outside of controllablecharacter handled with BP AI
#pragma once

#include "GameFramework/Character.h"
#include "DamageableInterface.h"
#include "CombatTextActor.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ALIZARINGAME_API ABaseCharacter : public ACharacter, public IDamageableInterface
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

	// flag for losing all health
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Health")
		bool isDead = false;

	// number of times health can be reduced to zero before being destroyed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
		int remainingLives = 0;

	// called whenever something loses a life
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
		void loseLife();

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

	// regeneration during active recharge, per second
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
		float shieldRechargeRate = 0; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		float plainDamageRate = 0; // generic damage over time

	
	UFUNCTION(BlueprintCallable, Category = "Damage")
		void restoreHealth();

	// implementation of interface requires odd _Implementation syntax
	// Unreal Macromagics, UFUNCTION shenanigans handled in DamageableInterface
	virtual void CalculateDamage_Implementation(float damage) override;
	
	virtual void TickDamage_Implementation(float deltaSeconds) override;

	// the type of floating combat text used for this character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<ACombatTextActor> FCT;
};
