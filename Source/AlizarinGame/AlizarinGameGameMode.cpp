// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "AlizarinGameGameMode.h"
#include "ControllableCharacter.h"


AAlizarinGameGameMode::AAlizarinGameGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// FClassFinder will search through the game's blueprint classes to find the player character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/MainPlayerCharacter"));
	
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

