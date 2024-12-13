// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ReclusionCharacterMovement.generated.h"

/**
 * Primary character movement class.
 */
UCLASS()
class RECLUSION_API UReclusionCharacterMovement : public UCharacterMovementComponent {

	GENERATED_UCLASS_BODY()

private:

	virtual float GetMaxSpeed() const override;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

};