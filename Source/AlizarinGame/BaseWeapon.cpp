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

void ABaseWeapon::FireHold_Implementation()
{
	// only fire when timer allows
	if (remainingShotDelay > 0.01) {
		if (debug) GEngine->AddOnScreenDebugMessage(-1, 5.f,
			FColor::Blue,
			FString::Printf(TEXT("Weapon remaining timer: %f"), remainingShotDelay));
		return;
	}

	FVector origin = GetActorRotation().RotateVector(barrelLocation) + GetActorLocation();
	FVector direction = GetActorRotation().Vector();
	FVector end = origin + range * direction;

	// traces against level and characters
	ECollisionChannel channel = ECC_WorldDynamic; 

	if (debug) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f,
			FColor::Blue,
			TEXT("Weapon confirmed fire"));
	}
	

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
			if (debug) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f,
					FColor::Blue,
					TEXT("Weapon spawning beam"));
			}
			// spawn a particle beam
			UParticleSystemComponent* spawnedBeam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				beam, 
				GetActorTransform(), 
				false); 

			if (spawnedBeam) {
				spawnedBeam->SetBeamSourcePoint(1, origin, 1);
				spawnedBeam->SetBeamEndPoint(1, end);
				if (debug) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f,
						FColor::Blue,
						TEXT("Weapon has spawned a beam"));
				}
			}
			else if (debug) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f,
					FColor::Blue,
					TEXT("Beam resulted in NULL"));
			}
		}
		
	}
	else {

	}
	

	remainingShotDelay = shotDelay;
}

void ABaseWeapon::FireRelease_Implementation()
{
	if (!chargeWeapon) return;
}