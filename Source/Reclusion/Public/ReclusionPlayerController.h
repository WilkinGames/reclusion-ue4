// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ReclusionPlayerController.generated.h"

/**
 * Primary player controller class.
 */
UCLASS(config=Game, Blueprintable)
class RECLUSION_API AReclusionPlayerController : public APlayerController {

	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(Transient)
	uint8 bInfiniteAmmo : 1;

	UPROPERTY(Transient)
	uint8 bInfiniteClip : 1;

	UPROPERTY(Transient)
	uint8 bHealthRegen : 1;

	UPROPERTY(Transient)
	uint8 bGodMode : 1;

	virtual void SetupInputComponent() override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
};