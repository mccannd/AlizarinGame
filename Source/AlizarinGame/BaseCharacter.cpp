
#include "AlizarinGame.h"
#include "BaseCharacter.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	IDamageableInterface::Execute_TickDamage(this, DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

// damage is assumed to be a positive number subtracted from health
void ABaseCharacter::CalculateDamage_Implementation(float damage)
{
	if (isDead) return; // stop, stop! it's already dead!

	// must have some sort of viable damage to prevent shield regeneration
	if (damage > 0.01) shieldRechargeDelayRemaining = shieldRechargeDelay;

	// health in this game obeys a "shield gate" mechanic to avoid instant death
	// if damage breaks a shield, excess damage does NOT ordinarily affect health
	// this may change in the future, perhaps with a penalty
	if (currentShield > 0) {
		currentShield -= damage;
		if (currentShield < 0) currentShield = 0;
		percentageShield = currentShield / maximumShield;
	}
	else {
		currentHealth -= damage;
		if (currentHealth < 0) currentHealth = 0;
		if (currentHealth < 0.01) isDead = true; // destruction may be in BP
		percentageHealth = currentHealth / maximumHealth;
	}
}

void ABaseCharacter::TickDamage_Implementation(float deltaSeconds) 
{
	// first apply any damage over time effects
	IDamageableInterface::Execute_CalculateDamage(this, plainDamageRate * deltaSeconds);

	// handle shield recharge
	shieldRechargeDelayRemaining -= deltaSeconds;
	if (shieldRechargeDelayRemaining <= 0) shieldRechargeDelayRemaining = 0;
	if (maximumShield > 0 && shieldRechargeDelayRemaining < 0.01) {
		currentShield += shieldRechargeRate * deltaSeconds;
		if (currentShield > maximumShield) currentShield = maximumShield;
		percentageShield = currentShield / maximumShield;
	}

	// regenerate health
	currentHealth += healthRegeneration * deltaSeconds;
	if (currentHealth > maximumHealth) currentHealth = maximumHealth;
	if (currentHealth < 0.01) {
		isDead = true; // possible for shield piercing effects like poison
		currentHealth = 0;
	}

	currentShield += shieldRegeneration * deltaSeconds;
	if (currentShield > maximumShield) currentShield = maximumShield;
	if (currentShield < 0.01) currentShield = 0;

}