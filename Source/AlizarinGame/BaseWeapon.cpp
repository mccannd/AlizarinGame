// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "BaseWeapon.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	remainingShotDelay -= DeltaTime;
	if (remainingShotDelay < 0) remainingShotDelay = 0;
}

void ABaseWeapon::FireHold()
{
	// only fire when timer allows
	if (remainingShotDelay > 0.01) return;

	if (!barrel) return;
	FVector origin = barrel->GetComponentLocation();
	FVector direction = barrel->GetComponentRotation().Vector();
	FVector end = origin + range * direction;

	// traces against level and characters
	ECollisionChannel channel = ECC_WorldDynamic; 

	// do not request any additional details from collision
	FCollisionQueryParams params(false);

	if (hitscanWeapon) {
		// trace a ray from weapon to range
		FHitResult collision = FHitResult(ForceInit);
		GetWorld()->LineTraceSingleByChannel(
			collision, 
			origin, 
			end, 
			channel, 
			params);

		// need to get damageable interface working first
	}
	else {

	}
	

	remainingShotDelay = shotDelay;
}

void ABaseWeapon::FireRelease()
{
	if (!chargeWeapon) return;
}