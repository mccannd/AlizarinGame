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
	// for some reason GetWorld() doesnt actually seem to work from Tick
	validWorld = GetWorld();
	
}

// Called every frame
void ABaseWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	remainingShotDelay -= DeltaTime;
	
	if (remainingShotDelay < 0) remainingShotDelay = 0;

	if (autoFireOn) FireHold(); //IWeaponInterface::Execute_FireHold(this);
}


void ABaseWeapon::SetOwningCharacter(AActor *newChar)
{
	owningCharacter = newChar;
	GEngine->AddOnScreenDebugMessage(-1, 5.f,
		FColor::Blue,
		TEXT("Set an owning character"));
}

//void ABaseWeapon::FireHold_Implementation()
void ABaseWeapon::FireHold()
{
	autoFireOn = true;

	// only fire when timer allows
	if (remainingShotDelay > 0.01) {
		return;
	}

	FVector origin = validRotation.RotateVector(barrelLocation) + GetActorLocation();
	FVector direction = validRotation.Vector();
	FVector end = origin + range * direction;

	// traces against level and characters
	ECollisionChannel channel = ECC_WorldDynamic;

	// do not request any additional details from collision
	FCollisionQueryParams params(false);

	if (hitscanWeapon) {
		// trace a ray from weapon to range
		FHitResult collision = FHitResult(ForceInit);
		validWorld->LineTraceSingleByChannel(collision, origin, end, channel, params);

		if (collision.IsValidBlockingHit()) {
			// need to get damageable interface working first to inflict damage
			end = collision.ImpactPoint;
			if (debug) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f,
					FColor::Blue,
					TEXT("Weapon confirmed collision"));
			}
		}

		if (beam) {
			// spawn a particle beam
			UParticleSystemComponent* spawnedBeam = 
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), beam, 
					GetActorTransform(), false);

			if (spawnedBeam) {
				spawnedBeam->SetBeamSourcePoint(0, origin, 0);
				spawnedBeam->SetBeamEndPoint(0, end);
			}
		}

		if (impact && collision.IsValidBlockingHit()) {
			UParticleSystemComponent* spawnedImpact = 
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impact,
					FTransform(FRotator(), end), false);
		}

		if (muzzleFlash) { 
			UParticleSystemComponent* spawnedFlash =
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleFlash,
					FTransform(GetActorRotation(), origin), false);
		}

	}
	else {

	}

	remainingShotDelay = shotDelay;
}

//void ABaseWeapon::FireRelease_Implementation()
void ABaseWeapon::FireRelease()
{
	autoFireOn = false;
	if (!chargeWeapon) return;
}