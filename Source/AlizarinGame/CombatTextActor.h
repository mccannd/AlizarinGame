// A single piece of floating information above a character
// Carries a string or numerical information and destroys itself after some time
#pragma once

#include "GameFramework/Actor.h"
#include "CombatTextActor.generated.h"

UCLASS()
class ALIZARINGAME_API ACombatTextActor : public AActor
{
	GENERATED_BODY()
	

private:
	FVector offset = FVector(0, 0, 100);
public:	
	// Sets default values for this actor's properties
	ACombatTextActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// the content of the floating combat text
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FCT")
		FString content;

	// the character that the text will follow, if there is one
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "FCT")
		AActor* subject;

	// time in seconds that this actor will persist
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FCT")
		float maxLifeSpan = 2;

	// time in seconds until actor is destroyed
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "FCT")
		float remainingLifeSpan = 2;
	
	// how quickly the FCT moves in world units / sec
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FCT")
		float speed = 25;

	// sets the content of the FCT to a number
	UFUNCTION(BlueprintCallable, Category = "FCT")
		void setFCTContent(FString text);

	// sets the actor that the FCT will follow
	UFUNCTION(BlueprintCallable, Category = "FCT")
		void setFCTSubject(AActor* focus);
};
