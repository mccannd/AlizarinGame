// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "CombatTextActor.h"


// Sets default values
ACombatTextActor::ACombatTextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatTextActor::BeginPlay()
{
	Super::BeginPlay();

	remainingLifeSpan = maxLifeSpan;
	
}

// Called every frame
void ACombatTextActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	remainingLifeSpan -= DeltaTime;


	// animate the position of the FCT
	if (subject) {
		// follow the subject plus some existing offset
		offset = offset + FVector(0, 0, speed * DeltaTime);
		SetActorLocation(subject->GetActorLocation() + offset);

	}
	else {
		// subject died or is missing, simply increment position
		SetActorLocation(GetActorLocation() + FVector(0, 0, speed * DeltaTime));
	}

	// destroy if life has run out
	if (remainingLifeSpan <= 0) Destroy();
}

void ACombatTextActor::setFCTContent(FString text) {

	content = FString(text);
}

void ACombatTextActor::setFCTSubject(AActor* focus) {
	subject = focus;
}
