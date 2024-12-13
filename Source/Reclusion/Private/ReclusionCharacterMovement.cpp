// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionCharacterMovement.h"
#include "ReclusionCharacter.h"

UReclusionCharacterMovement::UReclusionCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	//...

}

float UReclusionCharacterMovement::GetMaxSpeed() const {

	float MaxSpeed = Super::GetMaxSpeed();

	const AReclusionCharacter* ReclusionCharacterOwner = Cast<AReclusionCharacter>(PawnOwner);

	if (ReclusionCharacterOwner) {

		if (ReclusionCharacterOwner->IsAiming()) {

			MaxSpeed *= ReclusionCharacterOwner->GetAimingSpeedModifier();

		}

		if (ReclusionCharacterOwner->IsRunning()) {

			MaxSpeed *= ReclusionCharacterOwner->GetRunningSpeedModifier();

		}

		if (ReclusionCharacterOwner->IsCrouching()) {

			MaxSpeed *= ReclusionCharacterOwner->GetCrouchingSpeedModifier();

		}

	}

	return MaxSpeed;

}

void UReclusionCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}