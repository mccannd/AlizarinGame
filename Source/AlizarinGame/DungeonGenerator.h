// Generates every level for Alizarin!
// Use by making a blueprint subclass
// then in the blueprint subclass, use the editor to fill with Room subclasses
#pragma once

#include "GameFramework/Actor.h"
#include "GeneralizedRoom.h"
#include "DungeonGenerator.generated.h"


// Enumeration for our generator:
// BLOCKED means there is a wall here and we cant connect in this direction
// OPEN means there isn't anything here and we have a chance to connect
// REQUIRED means there is a doorway here which must be resolved with a connection
enum DoorwayStatus { BLOCKED, OPEN, REQUIRED };

// Struct for our generator: the map will consist of an array of these
struct RoomCellStruct {

	RoomCellStruct() {}

	// All doorway boundaries are open on initialization
	DoorwayStatus north = OPEN;
	DoorwayStatus south = OPEN;
	DoorwayStatus east = OPEN;
	DoorwayStatus west = OPEN;

	bool initialized = false; // whether the maze has finished its work on this cell

	ARoom* cell_room; // reference to room spawned inside this cell

};

// Struct allowing us to make 2D TArrays
struct RoomRow {
	TArray<RoomCellStruct> roomColumns;
};

UCLASS()
class ALIZARINGAME_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	

private:
	// Every room is stored in a 2D array (see structs above
	TArray<RoomRow> all_rooms;
	int32 cells_x;
	int32 cells_y;

	// beginning coordinates for spawn
	int32 origin_x;
	int32 origin_y;

	// distance between centers of each cell in CM
	float cell_length = 3000.0f;

public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TArray< TSubclassOf<ARoom> > Room_1_Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TArray< TSubclassOf<ARoom> > Room_2_Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TArray< TSubclassOf<ARoom> > Room_3_Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TArray< TSubclassOf<ARoom> > Room_4_Doors;

	// As of right now everything will be generated when the level begins
	// later, it will be expanded to work with an "endless mode"

	// Generate the entire level, will be called on generation.
	UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
		void GenerateMaze(int32 x, int32 y, int32 start_x, int32 start_y);

	bool bigRoomFits(int32 x, int32 y,
		TArray<FVector2D>& allCells,
		FVector2D& enter, FVector2D& exit,
		FVector2D& entryDir, FVector2D& exitDir);

	void GenerateCell(int32 x, int32 y); //recursively cell generation

	AActor* playerReference = NULL;
	int playerX;
	int playerY;
	int prevPlayerX;
	int prevPlayerY;
	
	//set the player X and Y depending on its location
	UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
		void CalcPlayerCell();

	// get coordinates of the cell the player is in
	UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
		FVector2D GetPlayerCellLocation();

	// maps a world location to cell coordinates
	UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
		FVector2D WorldToCell(FVector location);
};
