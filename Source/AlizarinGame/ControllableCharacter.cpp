// Fill out your copyright notice in the Description page of Project Settings.

#include "AlizarinGame.h"
#include "ControllableCharacter.h"


// modifies a rotator to look at the mouse. returns true if successful, false otherwise
bool AControllableCharacter::findMouseRotation(FRotator& rotation)
{
	
	if (GetWorld()) {
		FVector mousePos = FVector();
		FVector mouseDir = FVector();

		APlayerController* control = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		bool mouseFound = control->DeprojectMousePositionToWorld(mousePos, mouseDir);
		if (!mouseFound) return false; // conversion failed
		
		FVector location = this->GetActorLocation();
		FVector worldUp = FVector(0, 0, 1);

		float num = FVector::DotProduct(FVector(0, 0, location.Z) - mousePos, worldUp);
		float denom = FVector::DotProduct(mouseDir, worldUp);

		FVector targetOnPlane = mouseDir * num / denom + mousePos;
		rotation = FRotationMatrix::MakeFromX(location - targetOnPlane).Rotator();
		rotation = FRotator(0, rotation.Yaw + 180, 0);

		return true;
	} 
	
	return false; // should never happen in game, but just in case
}

// Called when the game starts or when spawned
void AControllableCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We are using FPSCharacter!"));
	}
	// ensures that on creation, input will apply to this pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

void AControllableCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	// set up movement controls
	InputComponent->BindAxis(
		"HorizontalAxis", 
		this, 
		&AControllableCharacter::MoveHorizontal
		);
	InputComponent->BindAxis(
		"VerticalAxis",
		this,
		&AControllableCharacter::MoveVertical
		);

	// set up mouse controls
	InputComponent->BindAction(
		"WeaponPrimary", 
		IE_Pressed, 
		this,
		&AControllableCharacter::PrimaryFireHold
		);
	InputComponent->BindAction(
		"WeaponPrimary",
		IE_Released,
		this,
		&AControllableCharacter::PrimaryFireRelease
		);
}

void AControllableCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// handle movement
	FVector movementDirection = xMovementVector + yMovementVector;
	movementDirection.Normalize();

	if (movementDirection.Size() > 0.01f && Controller != NULL) {		
		bool debug = false;
		
		if (GEngine && debug)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, 
				FString::Printf(TEXT("Direction Vector: (%f, %f, %f)"),
					movementDirection.X,
					movementDirection.Y,
					movementDirection.Z));
		}
		
		AddMovementInput(movementDirection, 1);
	}

	// handle rotation towards mouse
	FRotator current = GetActorRotation();
	FRotator target = FRotator();
	bool foundTarget = findMouseRotation(target);
	if (foundTarget) {
		SetActorRotation(FMath::RInterpTo(current, target, DeltaSeconds, 90.0));
	}
}

// Movement: game camera is fixed at 45 degrees away from world coordinate system
// so any movement inputs are rotated appropriately 

void AControllableCharacter::MoveVertical(float AxisValue)
{
	yMovementVector = AxisValue * FVector(0.5f, 0.5f, 0.0f);
}

void AControllableCharacter::MoveHorizontal(float AxisValue)
{
	xMovementVector = AxisValue * FVector(0.5f, -0.5f, 0.0f);
}

void AControllableCharacter::PrimaryFireHold()
{
	if (activeWeapon != NULL) {
		ABaseWeapon* weapon = activeWeapon->GetDefaultObject<ABaseWeapon>();
		if (weapon) weapon->FireHold();
	}
}

void AControllableCharacter::PrimaryFireRelease()
{
	if (activeWeapon != NULL) {
		ABaseWeapon* weapon = activeWeapon->GetDefaultObject<ABaseWeapon>();
		if (weapon) weapon->FireRelease();
	}
}
