// Copyright DGordeeva 2016

#include "BuildingEscape.h"
#include "Grabber.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	//get player viewpoint this tick - where they're looking
	FVector FPlayerViewpointLocation;
	FRotator FPlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT FPlayerViewpointLocation,
		OUT FPlayerViewpointRotation
	);

	///Draw a red trace in the world to visualize
	FVector LineTraceEnd = FPlayerViewpointLocation + FPlayerViewpointRotation.Vector() * Reach;
	DrawDebugLine(
		GetWorld(),
		FPlayerViewpointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f
	);

	//Set up query params
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Linetrace (ray-cast) out to reach distance
	FHitResult LineTraceHit;

	GetWorld()->LineTraceSingleByObjectType(
		OUT LineTraceHit,
		FPlayerViewpointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what what we hit
	AActor* ActorHit = LineTraceHit.GetActor();
	if (ActorHit)
		{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()))
		}
}

