// This room will occupy more than one grid space
#pragma once

#include "Room.h"
#include "GeneralizedRoom.generated.h"


UCLASS()
class ALIZARINGAME_API AGeneralizedRoom : public ARoom
{
	GENERATED_BODY()
	
public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
		The cells that this room will occupy based on local coordinates
		(0m, 0m) in BP local space should be represented as (0, 0) here
		(30m, -30m) should be (1, -1) with current generator cell size of 30x30 m
		+X values are "West", +Y values are "North"
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connectivity")
		TArray<FVector2D> occupiedCells;
	
	// The coordinate of the cell that will contain the entrance doorway of the room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connectivity")
		FVector2D entranceCell;

	// The coordinate of the cell that will contain the exit doorway of the room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connectivity")
		FVector2D exitCell;
	/*
		should be a unit vector aligned with +-X OR +-Y
		represents where dungeon generator should step from entrance cell
		to find the connecting cell in the map
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connectivity")
		FVector2D entranceAdjacencyDirection;

	/*
		should be a unit vector aligned with +-X OR +-Y
		represents where dungeon generator should step from exit cell
		to find the connecting cell in the map
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connectivity")
		FVector2D exitAdjacencyDirection;

};
