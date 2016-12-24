// Copyright DGordeeva 2016

#include "BuildingEscape.h"
#include "Grabber.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
	
}

///Look for attached physics handle
void UGrabber::FindPhysicsHandleComponent() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Physics Handle missing on %s"), *GetOwner()->GetName());
	}
}

///Look for input component
void UGrabber::SetupInputComponent() {
	PlayerInput = GetOwner()->FindComponentByClass<UInputComponent>();
	if (PlayerInput) {
		///Bind the input axis
		PlayerInput->BindAction(
			"Grab",
			IE_Pressed,
			this,
			&UGrabber::Grab
		);
		PlayerInput->BindAction(
			"Grab",
			IE_Released,
			this,
			&UGrabber::Release
		);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Input Component missing on %s"), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab(){
	//Try and reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	if (!PhysicsHandle) { return; }
	///If we hit something then attach a physics handle
	if (ActorHit) {
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true //allow rotation
		);
	}
}

void UGrabber::Release() {
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!PhysicsHandle) { return; }
	//get player viewpoint this tick - where they're looking
	//if the physics handle is attached - "take" the object
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {
	//get player viewpoint this tick - where they're looking
	DrawDebugLine(
		GetWorld(),
		GetLineTraceStart(),
		GetLineTraceEnd(),
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
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	return LineTraceHit;
}

FVector UGrabber::GetLineTraceStart() {
	FVector FPlayerViewpointLocation;
	FRotator FPlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT FPlayerViewpointLocation,
		OUT FPlayerViewpointRotation
	);
	return FPlayerViewpointLocation;
}

FVector UGrabber::GetLineTraceEnd() {
	FVector FPlayerViewpointLocation;
	FRotator FPlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT FPlayerViewpointLocation,
		OUT FPlayerViewpointRotation
	);
	FVector LineTraceEnd = FPlayerViewpointLocation + FPlayerViewpointRotation.Vector() * Reach;
	return LineTraceEnd;
}