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

// Enumeration for picking generalized rooms:
// START is always the start room and must be placed at origin
// END is the goal / end room
// OTHER is the default
enum ObjectiveType { START, END, OTHER};

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

// Structs allowing us to make 2D TArrays
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

	ARoom* entryCell = NULL;

	// the number of cells currently generated
	int32 occupiedCells = 0;
	// the average number of doorways
	float avgDegree = 0;
	// the number of cells that will be along a path, ie min of graph
	// note that these must have degree of at least 2
	int32 minNumCells = 0;

	int32 projectedNumCells = 0;

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

	// collection of all rooms with objectives
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Generator Modules")
		TArray<TSubclassOf<AGeneralizedRoom>> ObjectiveRooms;

	// target average degree of the level / graph, higher means more branches
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
		float targetDensity = 2.5;

	// the list of objectives, in order
	TArray<AGeneralizedRoom*> allObjectives;
	TArray<TPair<int32, int32>> allObjEnter;
	TArray<TPair<int32, int32>> allObjExit;
	int currentObjectiveIndex = 0;

	// the next objective that the player needs to complete
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator Modules")
		AGeneralizedRoom* CurrentObjectiveRoom;

	// where the level will start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TSubclassOf < AGeneralizedRoom> StartRoom;

	// where the level will finish
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Modules")
		TSubclassOf < AGeneralizedRoom> EndRoom;

	// used to mark the paths with something visible for debugging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TSubclassOf < ARoom> testMarker;

	// As of right now everything will be generated when the level begins
	// later, it will be expanded to work with an "endless mode"

	// Generate the entire level, will be called on generation.
	UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
		void GenerateMaze(int32 x, int32 y, int32 start_x, int32 start_y);

	bool bigRoomFits(int32 x, int32 y,
		TArray<FVector2D>& allCells,
		FVector2D& enter, FVector2D& exit,
		FVector2D& entryDir, FVector2D& exitDir);

	// place a generalized room in the scene and return a reference, NULL if fail
	AGeneralizedRoom* GenerateBigRoom(int32 x, int32 y,
		int32& xEnter, int32& yEnter, 
		int32& xExit, int32& yExit,
		ObjectiveType type = OTHER); 

	// finds a path between cell locations for recursive generation
	//UFUNCTION(BlueprintCallable, Category = "MazeGenerator")
	void GeneratePath(int32 x0, int32 y0, int32 x1, int32 y1);

	// recursive cell generation
	void GenerateCell(int32 x, int32 y); 

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
