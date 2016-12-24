// Copyright DGordeeva 2016

#include "BuildingEscape.h"
#include "OpenDoor.h"

#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("Pressure Plte missing on %s"), *GetOwner()->GetName());
	}
}

void UOpenDoor::OpenDoor() {
	Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}

void UOpenDoor::CloseDoor() {
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Poll the trigger volume every frame
	if (TotalMassOnPlate() > TriggerMass) { //TODO change the condition
		OpenDoor(); 
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	//check if it's time to close the door
	if (GetWorld()->GetTimeSeconds() >= (LastDoorOpenTime + DoorCloseDelay)) {
		CloseDoor();
	}
}

float UOpenDoor::TotalMassOnPlate() {
	float TotalMass = 0.f;
	//find all the overlapping actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { 
		return TotalMass; 
	}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	//loop through them, adding their masses
	for (const auto& Actor : OverlappingActors) {
		UE_LOG(LogTemp, Warning, TEXT("%s is on the Pressure Plate"), *(Actor->GetName()));
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}



