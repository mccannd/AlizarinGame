// The class from which all standard environmental room modules will derive from
// currently only has settings for connectivity, will later be able to spawn enemy encounters
#pragma once

#include "GameFramework/Actor.h"
#include "Room.generated.h"

UCLASS()
class ALIZARINGAME_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	// These booleans inform us whether there is a doorway in the specified cardinal direction
	// Each blueprint that derives from this class will have editable checkboxes, which must be filled properly
	// In editor coordinates: West is positive X. North is positive Y.
	
	// Connects in +Y direction?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connectivity")
		bool north = false;

	// Connects in -X direction?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connectivity")
		bool east = false;

	// Connects in -Y direction?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connectivity")
		bool south = false;

	// Connects in +X direction?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connectivity")
		bool west = false;
};
