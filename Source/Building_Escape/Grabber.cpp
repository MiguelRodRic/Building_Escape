// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Gameframework/Actor.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();

	FindInputComponent();
}

void UGrabber::FindInputComponent() {
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s    No input component found!"), *GetOwner()->GetName());
	}
}



void UGrabber::Grab() {

	//Line trace and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	//If we hit something, attach a physics handle
	if (ActorHit) {
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(), true);
	}
}

void UGrabber::Release() {
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s    No physics handle component found!"),
			*GetOwner()->GetName());
	}
}



// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector LineTracedEnd = GetReachLineEnd();
	// If  the physics handle is attached move the object holded
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTracedEnd);
	}
	
}

FVector UGrabber::GetReachLineEnd() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//Get player viewpoint
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);


	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetReachLineStart() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//Get player viewpoint
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);


	return PlayerViewPointLocation;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{

	//Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	//Line trace (Ray casting)
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, GetReachLineStart(), GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);

	AActor *HitActor = HitResult.GetActor();
	if (HitActor) {
		UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *HitActor->GetName());
	}

	return HitResult;
}

