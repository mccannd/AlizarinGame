// The base C++ class for player-equipped firearms
// Players will use blueprint subclasses of this class
#pragma once

#include "GameFramework/Actor.h"
#include "WeaponInterface.h"
#include "BaseWeapon.generated.h"

UCLASS()
class ALIZARINGAME_API ABaseWeapon : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
private:
	// timer until next weapon shot available
	float remainingShotDelay = 0.0;
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Enable Debug printing on screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool debug = false;

	// the name of the weapon
	// in the future:
	// weapons that are not unique will have prefixes and affixes associated to stat boosts
	// e.g. "Vicious Rocket Pod of Burning"
	// unique weapons will have their own name instead
	// e.g. "The Archon"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		FString name = "Default Weapon";

	// describes any statistical traits of this weapon in text
	// will be replaced with a proper printf later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		FString tooltip = "Default Weapon Tooltip";

	// extra flavor text, for explaining the purpose of the weapon or its origin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		FString flavortext = "";

	/// Interface implementation begins
	// the regular weapon firing, called repeatedly when mouse held down
	virtual void FireHold_Implementation() override;

	// called when releasing mouse, does nothing unless this is a "charge" weapon
	virtual void FireRelease_Implementation() override;

	/// interface implementation ends
	// where the weapon will fire from, a blueprint component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Functionality")
		FVector barrelLocation = FVector(0, 0, 0);

	// time between shots called by FireHold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float shotDelay = 1;

	// TO BE REPLACED WITH MORE COMPREHENSIVE STRUCT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float damageTEMP = 20;

	// true if the weapon only fires after mouse is held then released
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		bool chargeWeapon = false;

	// true if the weapon inflicts damage with raytrace as opposed to projectile
	// projectile is NYI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		bool hitscanWeapon = true;

	// maximum range of the weapon in units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		float range = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		UParticleSystem* beam = NULL;
};
