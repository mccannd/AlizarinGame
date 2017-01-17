
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

	if (FGenericPlatformMath::Abs(damage) < 0.001) return;

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
		if (currentHealth < 0.01) isDead = true; 
		percentageHealth = currentHealth / maximumHealth;
	}

	// implement floating combat text
	if (FCT) {
		UWorld* const World = GetWorld();
		if (World) {
			FTransform t = FTransform(GetActorLocation());
			
			ACombatTextActor* text = World->
				SpawnActor<ACombatTextActor>(FCT->GetDefaultObject()->GetClass(), t);

			if (text) {
				text->setFCTSubject(this);
				text->setFCTContent(FString::FromInt(FGenericPlatformMath::Round(damage)));
			}

		}
	}
	

	if (isDead) loseLife();

	
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

void ABaseCharacter::restoreHealth()
{
	currentShield = maximumShield;
	percentageShield = 1;
	currentHealth = maximumHealth;
	percentageHealth = 1;
	isDead = false;
}

// Called when a life is lost. Can be overwritten by a blueprint for specific characters
void ABaseCharacter::loseLife_Implementation()
{
	if (!isDead) return;
	
	if (remainingLives <= 1) {
		// No more lives, destroy the object after the end of the tick

		// could use an animation or particle effect option, NYI
		Destroy();
	}
	else {
		// reset the life of this character

		// probably need some other sort of indicator here
		remainingLives--;

		restoreHealth();
	}
}