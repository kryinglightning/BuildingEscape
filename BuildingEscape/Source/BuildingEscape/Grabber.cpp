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
	if (PhysicsHandle) {
		//everything OK
	}
	else {
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
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));
	//Try and reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	///If we hit something then attach a physics handle
	if (ActorHit) {
		///attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true //allow rotation
		);
	}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));
	PhysicsHandle->ReleaseComponent();
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
	FVector LineTraceEnd = FPlayerViewpointLocation + FPlayerViewpointRotation.Vector() * Reach;
	//if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
		//move the object that we're holding
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {
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
	return LineTraceHit;
}

