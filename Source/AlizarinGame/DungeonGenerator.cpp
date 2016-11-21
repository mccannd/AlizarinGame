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
	int northSouthCells = FMath::RandRange(6, 9);
	int eastWestCells = FMath::RandRange(6, 9);
	int startX = FMath::RandRange(1, 3);
	int startY = FMath::RandRange(1, 3);

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
			FString::Printf(TEXT("OriginCell is %d, %d"),
				startX,
				startY));
	}
	GenerateMaze(eastWestCells, northSouthCells, startX, startY);
}

// Called every frame
void ADungeonGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (playerReference == NULL) {
		playerReference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}

	// track player position and activate cells
	prevPlayerX = playerX;
	prevPlayerY = playerY;
	CalcPlayerCell();
	if (playerX != prevPlayerX || playerY != prevPlayerY) {
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
			FString::Printf(TEXT("Player Has Entered Cell %d, %d"),
				playerX,
				playerY));*/
		if (playerX >= 0 && playerX < cells_x && playerY >= 0 && playerY < cells_y) {
			ARoom* room = all_rooms[playerX].roomColumns[playerY].cell_room;
			if (room != nullptr) {
				//room->activateRoom();
				room->activateRoomBP();
			}
		}
		else {
			/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
				TEXT("No room in this cell"));*/
		}
	}
	// track room completion
	if (CurrentObjectiveRoom) {
		if (CurrentObjectiveRoom->isObjectiveComplete) {
			currentObjectiveIndex++;
			if (currentObjectiveIndex < allObjectives.Num()) {
				CurrentObjectiveRoom = allObjectives[currentObjectiveIndex];
				CurrentObjectiveRoom->isAccessible = true;
			}
		}
	}
	
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

	allObjectives = TArray<AGeneralizedRoom*>();

	// TODO: generate objectives and special cells
	int32 entryX, entryY, exitX, exitY;
	AGeneralizedRoom* entrance = GenerateBigRoom(origin_x, origin_y, 
		entryX, entryY, exitX, exitY, START);
	
	allObjectives.Add(entrance);
	TPair<int32, int32> p1 = TPair<int32, int32>();
	p1.Key = entryX;
	p1.Value = entryY;

	TPair<int32, int32> p2 = TPair<int32, int32>();
	p2.Key = exitX;
	p2.Value = exitY;
	allObjEnter.Add(TPair<int32, int32>(p1));
	allObjExit.Add(TPair<int32, int32>(p2));


	// generate up to 2 new rooms
	for (int i = 0; i < 2; i++) {
		int32 objX, objY, objX1, objY1;
		AGeneralizedRoom* objective = GenerateBigRoom(origin_x + 2, origin_y + 2, 
			objX, objY, objX1, objY1);

		if (objective) {
			p1.Key = objX;
			p1.Value = objY;
			p2.Key = objX1;
			p2.Value = objY1;

			allObjectives.Add(objective);
			allObjEnter.Add(TPair<int32, int32>(p1));
			allObjExit.Add(TPair<int32, int32>(p2));
		}
	}



	// generate a valid ending room
	AGeneralizedRoom* endingRoom = NULL;
	int32 endX, endY, endX1, endY1;
	while (endingRoom == NULL) {

		endingRoom = GenerateBigRoom(origin_x + 2, origin_y + 2,
			endX, endY, endX1, endY1, END);
	}

	allObjectives.Add(endingRoom);
	p1.Key = endX;
	p1.Value = endY;
	p2.Key = endX1;
	p2.Value = endY1;
	allObjEnter.Add(TPair<int32, int32>(p1));
	allObjExit.Add(TPair<int32, int32>(p2));


	// generate every path
	for (int i = 0; i < allObjEnter.Num() - 1; i++) {
		GeneratePath(allObjExit[i].Key, allObjExit[i].Value, 
			allObjEnter[i + 1].Key, allObjEnter[i + 1].Value);
	}

	// start recursive generation at all points
	for (int i = 0; i < allObjEnter.Num(); i++) {
		GenerateCell(allObjEnter[i].Key, allObjEnter[i].Value);
		GenerateCell(allObjExit[i].Key, allObjExit[i].Value);
	}

	currentObjectiveIndex = 0;
	CurrentObjectiveRoom = allObjectives[currentObjectiveIndex];
	CurrentObjectiveRoom->isAccessible = true;
	/*
	int32 objX, objY, objX1, objY1;
	GenerateBigRoom(origin_x + 2, origin_y + 2, objX, objY, objX1, objY1);


	GeneratePath(exitX, exitY, objX, objY);

	GenerateCell(entryX, entryY);

	GenerateCell(exitX, exitY);

	GenerateCell(objX, objY);

	GenerateCell(objX1, objY1);
	*/
	// Generate recursive paths along all_rooms struct
	//GenerateCell(start_x, start_y);
}

// helper for simple clockwise rotation, note that +x in unreal is reverse of usual +x
void clockwiseVector(FVector2D& vec)
{
	int temp = vec.X;
	vec.X = -vec.Y;
	vec.Y = temp;
}

// check if a big room fits at its current rotation at X and y
bool ADungeonGenerator::bigRoomFits(int32 x, int32 y,
	TArray<FVector2D>& allCells, 
	FVector2D& enter, FVector2D& exit,
	FVector2D& entryDir, FVector2D& exitDir)
{
	// check if any rooms are out of bounds
	// check if any rooms inside have been initialized
	// check if any adjacent cells are expecting a doorway
	for (FVector2D room : allCells) {
		int testX = FPlatformMath::RoundToInt(room.X) + x;
		int testY = FPlatformMath::RoundToInt(room.Y) + y;
		if (testX < 0 || testX >= cells_x) return false; // OOB
		if (testY < 0 || testY >= cells_y) return false; // OOB
		if (all_rooms[testX].roomColumns[testY].initialized) return false;

		bool isDoorway = room.Equals(enter);

		// possibly the ugliest code ever written by mortal hands
		if (testX + 1 < cells_x) {
			if (all_rooms[testX + 1].roomColumns[testY].east == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX + 1, testY).Equals(FVector2D(testX, testY) + entryDir)) return false;
				} else return false;
			}
		}

		if (testX - 1 >= 0) {
			if (all_rooms[testX - 1].roomColumns[testY].west == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX - 1, testY).Equals(FVector2D(testX, testY) + entryDir)) return false;
				}
				else return false;
			}
		}

		if (testY + 1 < cells_y) {
			if (all_rooms[testX].roomColumns[testY + 1].south == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX, testY + 1).Equals(FVector2D(testX, testY) + entryDir)) return false;
				}
				else return false;
			}
		}

		if (testY - 1 >= 0) {
			if (all_rooms[testX].roomColumns[testY - 1].north == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX, testY - 1).Equals(FVector2D(testX, testY) + entryDir)) return false;
				}
				else return false;
			}
		}

		for (int i = 0; i < allObjEnter.Num(); i++) {
			TPair<int, int> a = allObjEnter[i];
			TPair<int, int> b = allObjExit[i];

			if (testX == a.Key || testX == b.Key) return false;
			if (testY == a.Value || testY == b.Value) return false;
		}


	}
	// check if doorways lead to blocked areas
	// or are out of bounds
	int e1x = FPlatformMath::RoundToInt(enter.X) + FPlatformMath::RoundToInt(entryDir.X) + x;
	int e1y = FPlatformMath::RoundToInt(enter.Y) + FPlatformMath::RoundToInt(entryDir.Y) + y;
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

	int e2x = FPlatformMath::RoundToInt(exit.X) + FPlatformMath::RoundToInt(exitDir.X) + x;
	int e2y = FPlatformMath::RoundToInt(exit.Y) + FPlatformMath::RoundToInt(exitDir.Y) + y;
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
	// success!
	return true;
}

// reference parameters used for future spawning
AGeneralizedRoom* ADungeonGenerator::GenerateBigRoom(int32 x, int32 y,
	int32& xEnter, int32& yEnter, int32& xExit, int32& yExit, 
	ObjectiveType type)
{
	xEnter = -1;
	yEnter = -1;
	xExit = -1;
	yExit = -1;

	AGeneralizedRoom* room;
	if (type == START) {
		room = StartRoom->GetDefaultObject<AGeneralizedRoom>();
	}
	else if (type == END) {
		room = EndRoom->GetDefaultObject<AGeneralizedRoom>();
	}
	else {
		if (ObjectiveRooms.Num() == 0) {
			room = StartRoom->GetDefaultObject<AGeneralizedRoom>();
		}
		else {
			room = ObjectiveRooms[FMath::RandRange(0, 
				ObjectiveRooms.Num() - 1)]->GetDefaultObject<AGeneralizedRoom>();
		}
		
	}

	AGeneralizedRoom* spawned_room = NULL;

	// copy all properties for placement
	TArray<FVector2D> roomspace = TArray<FVector2D>(room->occupiedCells);
	FVector2D enter = FVector2D(room->entranceCell);
	FVector2D exit = FVector2D(room->exitCell);
	FVector2D enterDir = FVector2D(room->entranceAdjacencyDirection);
	FVector2D exitDir = FVector2D(room->exitAdjacencyDirection);

	// try to scatter near this point
	for (int tries = 8; tries > 0; tries--) {
		int x1 = x + FMath::RandRange(-2, 2);
		FMath::Clamp(x1, 0, cells_x - 1);
		int y1 = y + FMath::RandRange(-2, 2);
		FMath::Clamp(y1, 0, cells_y - 1);

		if (type == START) x1 = x;
		if (type == START) y1 = y;

		int startRotations = 0;
		/*
		//int startRotations = FMath::RandRange(0, 3);
		for (int i = 0; i < startRotations; i++) {
			clockwiseVector(enter);
			clockwiseVector(exit);
			clockwiseVector(enterDir);
			clockwiseVector(exitDir);
			for (int i = 0; i < roomspace.Num(); i++) {
				FVector2D c = roomspace[i];
				clockwiseVector(c);
				roomspace[i] = c;
			}
		
		}*/

		// try all rotations in this location
		for (int rotations = 0; rotations < 4; rotations++) {

			bool fit = bigRoomFits(x1, y1, 
				roomspace, enter, exit, enterDir, exitDir);

			if (fit) {
				// success! now spawn the room

				// make appropriate transform
				FVector location = FVector((x1 - origin_x) * cell_length,
					(y1 - origin_y) * cell_length, 0);
				FRotator rotation = FRotator(0, (rotations + startRotations)* 90.0f, 0);
				FTransform transform = FTransform(rotation, location);

				GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue,
					FString::Printf(TEXT("BigRoom Generated at %d, %d"),
						x1,
						y1));
				GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue,
					FString::Printf(TEXT("With rotations: %d"),
						rotations + startRotations));

				// spawn the room in this position
				
				UWorld* const World = GetWorld();
				if (World) {
					if (room) {

						spawned_room = 
							World->SpawnActor<AGeneralizedRoom>(room->GetClass(), transform);
						
					}
					else { UE_LOG(LogTemp, Warning, TEXT("Room is nullptr")); }
				}
				else { UE_LOG(LogTemp, Warning, TEXT("World is nullptr")); }

				// mark and prepare the grid cells
				for (int i = 0; i < roomspace.Num(); i++)  {
					FVector2D cell = roomspace[i];
					int currentX = FGenericPlatformMath::RoundToInt(cell.X) + x1;
					int currentY = FGenericPlatformMath::RoundToInt(cell.Y) + y1;

					GEngine->AddOnScreenDebugMessage(-1, 500.f, FColor::Green,
						FString::Printf(TEXT("Mark cell %d %d"),
							(int) currentX, (int) currentY));

					all_rooms[currentX].roomColumns[currentY].cell_room = spawned_room;
					all_rooms[currentX].roomColumns[currentY].initialized = true;

					all_rooms[currentX].roomColumns[currentY].north = BLOCKED;
					all_rooms[currentX].roomColumns[currentY].east = BLOCKED;
					all_rooms[currentX].roomColumns[currentY].south = BLOCKED;
					all_rooms[currentX].roomColumns[currentY].west = BLOCKED;

					if (cell.Equals(enter)) {
						if (enterDir.Equals(FVector2D(1, 0))) {
							all_rooms[currentX].roomColumns[currentY].west = REQUIRED;
						}
						else if (enterDir.Equals(FVector2D(0, 1))) {
							all_rooms[currentX].roomColumns[currentY].north = REQUIRED;
						}
						else if (enterDir.Equals(FVector2D(-1, 0))) {
							all_rooms[currentX].roomColumns[currentY].east = REQUIRED;
						}
						else {
							all_rooms[currentX].roomColumns[currentY].south = REQUIRED;
						}

						xEnter = x1 + FGenericPlatformMath::RoundToInt(cell.X + enterDir.X);
						yEnter = y1 + FGenericPlatformMath::RoundToInt(cell.Y + enterDir.Y);
					}
					if (cell.Equals(exit)) {
						if (exitDir.Equals(FVector2D(1, 0))) {
							all_rooms[currentX].roomColumns[currentY].west = REQUIRED;
						}
						else if (exitDir.Equals(FVector2D(0, 1))) {
							all_rooms[currentX].roomColumns[currentY].north = REQUIRED;
						}
						else if (exitDir.Equals(FVector2D(-1, 0))) {
							all_rooms[currentX].roomColumns[currentY].east = REQUIRED;
						}
						else {
							all_rooms[currentX].roomColumns[currentY].south = REQUIRED;
						}
						xExit = x1 + FGenericPlatformMath::RoundToInt(cell.X + exitDir.X);
						yExit = y1 + FGenericPlatformMath::RoundToInt(cell.Y + exitDir.Y);
					}

				}
				// return a reference to the room
				return spawned_room;
			}

			// if failed, next rotation
			clockwiseVector(enter);
			clockwiseVector(exit);
			clockwiseVector(enterDir);
			clockwiseVector(exitDir);
			for (int i = 0; i < roomspace.Num(); i++) {
				FVector2D c = roomspace[i];
				clockwiseVector(c);
				roomspace[i] = c;
			}
		}
	}
	return NULL; // failed to spawn a room
}

// because Unreal does not seem to like multidimensional arrays
struct RowPath {
	TArray<TPair<int32, int32>> p;
};

struct Row2D {
	TArray<RowPath> row;
};

// helper for changing and adding better paths
/* Inputs: 
pq is a queue of paths.
ctp is cells-to-paths, which contains the best path from some cell to all others
p is the current path we are looking at
x and y are the cell coordinates we are examining

Outputs:
returns true if p is inserted to ctp[x][y], false otherwise
*/
bool PathConnect(TArray<RowPath>& pq, TArray<Row2D>& ctp, 
	RowPath& p, int32 x, int32 y) {
	if (x < 0 || x >= ctp.Num()) return false;
	if (y < 0 || y >= ctp[0].row.Num()) return false;
	
	if (ctp[x].row[y].p.Num() == 0 ||
		p.p.Num() + 1 < ctp[x].row[y].p.Num()) {
		// if there is not yet a path here or there is a worse one
		TArray<TPair<int32, int32>> newpath = TArray<TPair<int32, int32>>(p.p);
		TPair<int32, int32> newpt = TPair<int32, int32>();
		newpt.Key = x; newpt.Value = y;
		newpath.Push(newpt);
		RowPath np = RowPath();
		np.p = newpath;
		pq.Push(np);

		ctp[x].row[y].p = newpath;
		return true;
	}
	return false;
}

/*
Do a BFS in order to connect two cells with required doorwaystatus paths

Inputs: x0, y0 are start coordinates. x1, y1 are end coordinates.

Outputs: void. will get path that reaches x1, y1 and mark cells appropriately
*/
void ADungeonGenerator::GeneratePath(int32 x0, int32 y0, int32 x1, int32 y1)
{
	if (x1 == -1 || y1 == -1) {
		return;
	}

	// construct a table of paths, same size as dungeon
	TArray<Row2D> cellsToPaths = TArray<Row2D>();
	// initialize empty cells
	for (int i = 0; i < cells_x; i++) {
		Row2D row = Row2D();
		for (int j = 0; j < cells_y; j++) {
			RowPath p = RowPath();
			p.p = TArray<TPair<int32, int32>>();
			row.row.Add(p);
		}
		cellsToPaths.Add(row);
	}

	// create a list which will queue next path to trace
	TArray<RowPath> pathQueue = TArray<RowPath>();

	TArray<TPair<int32, int32>> path = TArray<TPair<int32, int32>>();
	// starting from (x0, y0)
	TPair<int32, int32> origin = TPair<int32, int32>();
	origin.Key = x0;
	origin.Value = y0;
	path.Add(origin);
	
	cellsToPaths[x0].row[y0].p = path;
	RowPath rp = RowPath(); rp.p = path; pathQueue.Push(rp);
	// get next path from the queue
	while (pathQueue.Num() > 0) {
		RowPath current = pathQueue.Pop();
		TPair<int32, int32> currentEnd = current.p.Last();

		RoomCellStruct room;

		// check cells in all directions

		if (currentEnd.Key + 1 >= 0 && currentEnd.Key + 1 < all_rooms.Num()
			&& currentEnd.Value >= 0 && currentEnd.Value < all_rooms[0].roomColumns.Num()) {
			
			room = all_rooms[currentEnd.Key + 1].roomColumns[currentEnd.Value];
			// if connection is possible
			if (room.east != BLOCKED  && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key + 1, currentEnd.Value);
			}
		}

		if (currentEnd.Key - 1 >= 0 && currentEnd.Key - 1 < all_rooms.Num()
			&& currentEnd.Value >= 0 && currentEnd.Value < all_rooms[0].roomColumns.Num()) {

			room = all_rooms[currentEnd.Key - 1].roomColumns[currentEnd.Value];
			// if connection is possible
			if (room.west != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key - 1, currentEnd.Value);
			}
		}


		if (currentEnd.Key>= 0 && currentEnd.Key< all_rooms.Num()
			&& currentEnd.Value + 1 >= 0 && currentEnd.Value + 1 < all_rooms[0].roomColumns.Num()) {

			room = all_rooms[currentEnd.Key].roomColumns[currentEnd.Value + 1];
			// if connection is possible
			if (room.south != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key, currentEnd.Value + 1);
			}
		}

		if (currentEnd.Key>= 0 && currentEnd.Key< all_rooms.Num()
			&& currentEnd.Value - 1 >= 0 && currentEnd.Value - 1 < all_rooms[0].roomColumns.Num()) {

			room = all_rooms[currentEnd.Key].roomColumns[currentEnd.Value - 1];
			// if connection is possible
			if (room.north != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key, currentEnd.Value - 1);
			}

		}
	}

	// mark the final path with required doorways
	TArray<TPair<int32, int32>> finalPath = cellsToPaths[x1].row[y1].p;
	for (int i = 0; i < finalPath.Num(); i++) {
		TPair<int32, int32> curr = finalPath[i];

		if (i > 0) {
			TPair<int32, int32> prev = finalPath[i - 1];
			RoomCellStruct r = all_rooms[curr.Key].roomColumns[curr.Value];
			if (prev.Key + 1 == curr.Key) {
				r.east = REQUIRED;
			}
			else if (prev.Key - 1 == curr.Key) {
				r.west = REQUIRED;
			}
			else if (prev.Value + 1 == curr.Value) {
				r.south = REQUIRED;
			}
			else if (prev.Value - 1 == curr.Value) {
				r.north = REQUIRED;
			}


			all_rooms[curr.Key].roomColumns[curr.Value] = r;
		}

		if (i < finalPath.Num() - 1) {
			TPair<int32, int32> next = finalPath[i + 1];
			RoomCellStruct r = all_rooms[curr.Key].roomColumns[curr.Value];
			if (next.Key == curr.Key + 1) {
				r.east = REQUIRED;
			}
			else if (next.Key == curr.Key - 1) {
				r.west = REQUIRED;
			}
			else if (next.Value == curr.Value - 1) {
				r.south = REQUIRED;
			}
			else if (next.Value == curr.Value + 1) {
				r.north = REQUIRED;
			}


			all_rooms[curr.Key].roomColumns[curr.Value] = r;
		}
		/*GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan,
			FString::Printf(TEXT("Path through: %d, %d"),
				curr.Key,
				curr.Value));*/
		
		// debugging which will trace path
		if (testMarker) {
			FTransform transform = FTransform(FVector((curr.Key - origin_x) * cell_length,
				(curr.Value - origin_y) * cell_length, 500));
			UWorld* const World = GetWorld();
			if (World) {
				ARoom* m = World->SpawnActor<ARoom>(testMarker->GetDefaultObject()->GetClass(), transform);

				if (!m) {
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,
						TEXT("ECH"));
				}
			}
		}
		

		
	}

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
	if (x < 0 || y < 0) return;

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

FVector2D ADungeonGenerator::WorldToCell(FVector location)
{
	int dx = FGenericPlatformMath::Floor((location.X + cell_length / 2.0f) / cell_length);
	int dy = FGenericPlatformMath::Floor((location.Y + cell_length / 2.0f) / cell_length);
	return FVector2D(origin_x + dx, origin_y + dy);
}

void ADungeonGenerator::CalcPlayerCell()
{
	if (playerReference == NULL) return;
	FVector2D location = WorldToCell(playerReference->GetActorLocation());
	playerX = (int)location.X;
	playerY = (int)location.Y;
}

FVector2D ADungeonGenerator::GetPlayerCellLocation()
{
	return FVector2D(playerX, playerY);
}