// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "ControllableCharacter.h"


// Called when the game starts or when spawned
void AControllableCharacter::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}