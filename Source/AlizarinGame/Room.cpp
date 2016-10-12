// The class from which all standard environmental room modules will derive from
// currently only has settings for connectivity, will later be able to spawn enemy encounters
// .cpp currently only has default actor stuff

#include "AlizarinGame.h"
#include "Room.h"


// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoom::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


void ARoom::activateRoom()
{
	if (previouslyActivated) return;
	else previouslyActivated = true;

	// spawn ze placeholdah
	UWorld* const World = GetWorld();
	if (World) {
		if (spawnEnemyTEMP) {
			FTransform t = FTransform(GetTransform());
			t.AddToTranslation(FVector(100, 100, 100));
			ACharacter* spawnedChar = World->SpawnActor<ACharacter>(spawnEnemyTEMP->GetClass(), t);
			if (spawnedChar == NULL) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f,
					FColor::Blue,
					TEXT("Spawning Failed"));
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f,
				FColor::Blue,
				TEXT("No character to spawn"));
		}
	}
	else if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f,
			FColor::Blue,
			TEXT("World returned NULL"));
	}
}
