// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "GeneralizedRoom.h"


// Called when the game starts or when spawned
void AGeneralizedRoom::BeginPlay()
{
	Super::BeginPlay();


	// verify the validity of the entrance and exit inputs
	// (should not fold in on itself)
	for (FVector2D coord : occupiedCells) {
		FVector2D testEntry = entranceCell + entranceAdjacencyDirection;
		FVector2D testExit = exitCell + exitAdjacencyDirection;
		verify(!(coord.Equals(testEntry) || coord.Equals(testExit)));
	}
}

