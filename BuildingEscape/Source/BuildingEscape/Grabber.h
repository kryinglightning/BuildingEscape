// Copyright DGordeeva 2016

#pragma once

#include "Components/ActorComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	//how far can the player reach
	float Reach = 100.f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* PlayerInput = nullptr;
	
	//raycast and grab what's in reach
	void Grab();
	
	void Release();

	void FindPhysicsHandleComponent();

	void SetupInputComponent();

	//get the 1st physics body in reach 
	const FHitResult GetFirstPhysicsBodyInReach();

};
