// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "ReclusionPlayerCameraManager.generated.h"

/**
 * Primary player camera manager class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionPlayerCameraManager : public APlayerCameraManager {

	GENERATED_UCLASS_BODY()

protected:

	FVector WeaponLocationDelta;

	FRotator WeaponRotationDelta;

	UPROPERTY(VisibleDefaultsOnly, Category=Player)
	FVector WeaponBaseLocation;

	uint32 bPendingWeaponKick;

	FVector WeaponKick;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float IdleFieldOfView;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float RunningFieldOfViewMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	uint32 bEnableWeaponSway : 1;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float WeaponSwayMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float WeaponSwayRotationMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float WeaponSwayInterpAlpha;

	virtual void PostInitializeComponents() override;

public:

	virtual void UpdateCamera(float DeltaTime) override;

	void AddWeaponKick(FVector NewWeaponKick);
	
};