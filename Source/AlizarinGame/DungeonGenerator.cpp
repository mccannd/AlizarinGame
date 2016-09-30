// Generates every level for Alizarin!
// Use by making a blueprint subclass
// then in the blueprint subclass, use the editor to fill with Room subclasses

#include "AlizarinGame.h"
#include "DungeonGenerator.h"


// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();

	// currently uses the naive recursive implementation
	// later, this will be replaced with the recursive call of each found path
	int northSouthCells = FMath::RandRange(5, 10);
	int eastWestCells = FMath::RandRange(5, 10);
	int startX = FMath::RandRange(2, 4);
	int startY = FMath::RandRange(2, 4);
	GenerateMaze(eastWestCells, northSouthCells, startX, startY);
}

// Called every frame
void ADungeonGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Initializes the generation of the levels
// (maybe a misnomer, isn't an actual "maze")
void ADungeonGenerator::GenerateMaze(int32 x, int32 y, int32 start_x, int32 start_y)
{
	cells_x = x;
	cells_y = y;

	// fill with baseline, empty rows
	for (int i = 0; i < x; i++) {
		RoomRow newRow = RoomRow();
		for (int j = 0; j < y; j++) {
			RoomCellStruct newRoom = RoomCellStruct();
			newRow.roomColumns.Add(newRoom);
		}
		all_rooms.Add(newRow);
	}

	// begin generating maze
	origin_x = start_x;
	origin_y = start_y;

	// TODO: generate objectives and special cells

	// Generate recursive paths along all_rooms struct
	GenerateCell(start_x, start_y);
}

// helper for testing orientationg of big room candidates
void rotateBigCW(TArray<FVector2D>& cells) {
	for (FVector2D cell : cells) {
		int temp = cell.X;
		cell.X = -cell.Y;
		cell.Y = temp;
	}
}

// check if a big room fits at its current rotation at X and y
bool ADungeonGenerator::bigRoomFits(int32 x, int32 y,
	TArray<FVector2D>& allCells, 
	FVector2D& enter, FVector2D& exit,
	FVector2D& entryDir, FVector2D& exitDir)
{
	// check if any rooms are out of bounds
	// check if any rooms inside have been initialized
	for (FVector2D room : allCells) {
		int testX = (int)room.X + x;
		int testY = (int)room.Y + y;
		if (testX < 0 || testX >= cells_x) return false; // OOB
		if (testY < 0 || testY >= cells_y) return false; // OOB
		if (all_rooms[testX].roomColumns[testY].initialized) return false;
	}
	// check if doorways lead to blocked areas
	int e1x = (int)enter.X + (int)entryDir.X + x;
	int e1y = (int)enter.Y + (int)entryDir.Y + y;
	if (e1x < 0 || e1x >= cells_x) return false;
	if (e1y < 0 || e1y >= cells_y) return false;
	if (entryDir.Equals(FVector2D(1, 0))) {
		if (all_rooms[e1x].roomColumns[e1y].east == BLOCKED) return false;
	} else if (entryDir.Equals(FVector2D(-1, 0))) {
		if (all_rooms[e1x].roomColumns[e1y].west == BLOCKED) return false;
	} else if (entryDir.Equals(FVector2D(0, 1))) {
		if (all_rooms[e1x].roomColumns[e1y].south == BLOCKED) return false;
	} else if (entryDir.Equals(FVector2D(0, -1))) {
		if (all_rooms[e1x].roomColumns[e1y].north == BLOCKED) return false;
	}

	int e2x = (int)exit.X + (int)exitDir.X + x;
	int e2y = (int)exit.Y + (int)exitDir.Y + y;
	if (e2x < 0 || e2x >= cells_x) return false;
	if (e2y < 0 || e2y >= cells_y) return false;
	if (exitDir.Equals(FVector2D(1, 0))) {
		if (all_rooms[e2x].roomColumns[e2y].east == BLOCKED) return false;
	}
	else if (exitDir.Equals(FVector2D(-1, 0))) {
		if (all_rooms[e2x].roomColumns[e2y].west == BLOCKED) return false;
	}
	else if (exitDir.Equals(FVector2D(0, 1))) {
		if (all_rooms[e2x].roomColumns[e2y].south == BLOCKED) return false;
	}
	else if (exitDir.Equals(FVector2D(0, -1))) {
		if (all_rooms[e2x].roomColumns[e2y].north == BLOCKED) return false;
	}

	return true;
}

// helper for testing orientations of room candidates
void rotateCW(bool& n, bool& e, bool& s, bool& w)
{
	bool t = w;
	w = s;
	s = e;
	e = n;
	n = t;
}

// check if a room fits into its position,
// given the status of adjacent doors
// given whether it is trying to put a door at each side
bool roomFits(DoorwayStatus adj_n,
	DoorwayStatus adj_e,
	DoorwayStatus adj_s,
	DoorwayStatus adj_w,
	bool n, bool e, bool s, bool w)
{
	// false if trying to put a door in a blocked location
	if (n && adj_n == DoorwayStatus::BLOCKED) return false;
	if (e && adj_e == DoorwayStatus::BLOCKED) return false;
	if (s && adj_s == DoorwayStatus::BLOCKED) return false;
	if (w && adj_w == DoorwayStatus::BLOCKED) return false;

	// false if not putting a door in a required location
	if (!n && adj_n == DoorwayStatus::REQUIRED) return false;
	if (!e && adj_e == DoorwayStatus::REQUIRED) return false;
	if (!s && adj_s == DoorwayStatus::REQUIRED) return false;
	if (!w && adj_w == DoorwayStatus::REQUIRED) return false;

	return true; // otherwise good to go
}

// generates a single cell of the dungeon recursively
// recursion occurs on each door of newly generated room
// base case is reaching boundary (shouldn't happen) or a cell with a room already in it
void ADungeonGenerator::GenerateCell(int32 x, int32 y) {
	// return if going out of bounds
	if (x >= all_rooms.Num()) return;
	if (y >= all_rooms[0].roomColumns.Num()) return;

	RoomCellStruct currentCell = all_rooms[x].roomColumns[y];

	// if this room was already generated, go back (base case)
	if (currentCell.initialized) return; 

	// status of each adjacent cell: BLOCKED (cannot connect) REQUIRED (must) OPEN (can)
	DoorwayStatus adj_w, adj_e, adj_s, adj_n;

	// see if boundary is nearby, otherwise look into adjacent cell
	if (x <= 0) adj_e = DoorwayStatus::BLOCKED;
	else adj_e = all_rooms[x - 1].roomColumns[y].west;

	if (x >= cells_x - 1) adj_w = DoorwayStatus::BLOCKED;
	else adj_w = all_rooms[x + 1].roomColumns[y].east;

	if (y <= 0) adj_s = DoorwayStatus::BLOCKED;
	else adj_s = all_rooms[x].roomColumns[y - 1].north;

	if (y >= cells_y - 1) adj_n = DoorwayStatus::BLOCKED;
	else adj_n = all_rooms[x].roomColumns[y + 1].south;

	// determine how many doorways the next room can / will have
	int min = 0;
	if (adj_s == DoorwayStatus::REQUIRED) min++;
	if (adj_n == DoorwayStatus::REQUIRED) min++;
	if (adj_e == DoorwayStatus::REQUIRED) min++;
	if (adj_w == DoorwayStatus::REQUIRED) min++;
	int max = 4;
	if (adj_s == DoorwayStatus::BLOCKED) max--;
	if (adj_n == DoorwayStatus::BLOCKED) max--;
	if (adj_e == DoorwayStatus::BLOCKED) max--;
	if (adj_w == DoorwayStatus::BLOCKED) max--;

	int num_doorways = FMath::RandRange(min, max);
	if (num_doorways == 0) num_doorways++;

	// to be removed later
	bool debuggerOn = false;

	if (debuggerOn) {
		UE_LOG(LogTemp, Warning, TEXT("Blocked Directions: %s%s%s%s"),
			adj_n == DoorwayStatus::BLOCKED ? TEXT("North ") : TEXT(""),
			adj_e == DoorwayStatus::BLOCKED ? TEXT("East ") : TEXT(""),
			adj_s == DoorwayStatus::BLOCKED ? TEXT("South ") : TEXT(""),
			adj_w == DoorwayStatus::BLOCKED ? TEXT("West ") : TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Required Directions: %s%s%s%s"),
			adj_n == DoorwayStatus::REQUIRED ? TEXT("North ") : TEXT(""),
			adj_e == DoorwayStatus::REQUIRED ? TEXT("East ") : TEXT(""),
			adj_s == DoorwayStatus::REQUIRED ? TEXT("South ") : TEXT(""),
			adj_w == DoorwayStatus::REQUIRED ? TEXT("West") : TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("Selected %d number of doorways"), num_doorways);
	}


	// select a random room with this number of doorways
	int num_room_of_type;
	if (num_doorways == 1) num_room_of_type = Room_1_Door.Num();
	else if (num_doorways == 2) num_room_of_type = Room_2_Doors.Num();
	else if (num_doorways == 3) num_room_of_type = Room_3_Doors.Num();
	else num_room_of_type = Room_4_Doors.Num();

	// make the set of possible indices for rooms of this size
	TArray<int> index_selector;
	for (int i = 0; i < num_room_of_type; i++) index_selector.Add(i);

	// will be populated with the adjacency info of candidate rooms
	bool n = false;
	bool e = false;
	bool s = false;
	bool w = false;

	// if generation has succeeded
	bool success = false;
	// number of attempts remaining for this cell, otherwise will abort
	int remainingTries = 5;

	while (index_selector.Num() > 0 && remainingTries > 0 && !success) {
		remainingTries--;

		// select a random room that hasnt been tried yet
		int selected = FMath::RandRange(0, index_selector.Num() - 1);
		selected = index_selector[selected];
		ARoom* room;
		if (num_doorways == 1)  room = Room_1_Door[selected]->GetDefaultObject<ARoom>();
		else if (num_doorways == 2) room = Room_2_Doors[selected]->GetDefaultObject<ARoom>();
		else if (num_doorways == 3) room = Room_3_Doors[selected]->GetDefaultObject<ARoom>();
		else room = Room_4_Doors[selected]->GetDefaultObject<ARoom>();

		// fill in adjacency information
		n = room->north;
		e = room->east;
		s = room->south;
		w = room->west;

		// begin with a random initial rotation
		int initialRotations = FMath::RandRange(0, 3);
		for (int i = 0; i < initialRotations; i++) {
			rotateCW(n, e, s, w);
		}

		// since doors are only in cardinal directions, only 4 tries necessary
		for (int rotations = 0; rotations < 4; rotations++) {
			if (debuggerOn) {
				UE_LOG(LogTemp, Warning, TEXT("Trying rotation %d at %d, %d: %s%s%s%s"),
					rotations, x, y,
					n ? TEXT("North ") : TEXT(""),
					e ? TEXT("East ") : TEXT(""),
					s ? TEXT("South ") : TEXT(""),
					w ? TEXT("West ") : TEXT(""));
			}

			// check for fit
			success = roomFits(adj_n, adj_e, adj_s, adj_w, n, e, s, w);

			if (success) {
				// make appropriate transform
				FVector location = FVector((x - origin_x) * cell_length,
					(y - origin_y) * cell_length, 0);
				FRotator rotation = FRotator(0, (rotations + initialRotations)* 90.0f, 0);
				FTransform transform = FTransform(rotation, location);

				currentCell.initialized = true;

				// spawn the room in this position
				UWorld* const World = GetWorld();
				if (World) {
					if (room) {
						ARoom* spawned_room = World->SpawnActor<ARoom>(room->GetClass(), transform);
						currentCell.cell_room = spawned_room;
					}
					else { if (debuggerOn) UE_LOG(LogTemp, Warning, TEXT("Room is nullptr")); }
				}
				else { if (debuggerOn) UE_LOG(LogTemp, Warning, TEXT("World is nullptr")); }

				//mark the doorways of this cell for the next cells
				if (n) currentCell.north = DoorwayStatus::REQUIRED;
				else currentCell.north = DoorwayStatus::BLOCKED;
				if (e) currentCell.east = DoorwayStatus::REQUIRED;
				else currentCell.east = DoorwayStatus::BLOCKED;
				if (s) currentCell.south = DoorwayStatus::REQUIRED;
				else currentCell.south = DoorwayStatus::BLOCKED;
				if (w) currentCell.west = DoorwayStatus::REQUIRED;
				else currentCell.west = DoorwayStatus::BLOCKED;

				// should prob use refs
				all_rooms[x].roomColumns[y] = currentCell;

				break;
			}
			// next rotation
			rotateCW(n, e, s, w);
		}
		
		index_selector.Remove(selected); // room placement failed, take this choice out of the list and move on
	}

	// generate new cells from each doorway placed
	// cells are in final orientation because or rotateCW helper
	if (success) {
		if (n) GenerateCell(x, y + 1);
		if (e) GenerateCell(x - 1, y);
		if (s) GenerateCell(x, y - 1);
		if (w) GenerateCell(x + 1, y);
	}
	else if (debuggerOn) {
		UE_LOG(LogTemp, Warning, TEXT("Cell generation failed"));
	}
}