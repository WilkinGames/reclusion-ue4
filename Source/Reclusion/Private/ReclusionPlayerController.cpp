// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionPlayerController.h"
#include "ReclusionPlayerCameraManager.h"

AReclusionPlayerController::AReclusionPlayerController(const FObjectInitializer& PCIP)
	: Super(PCIP) {

	PlayerCameraManagerClass = AReclusionPlayerCameraManager::StaticClass();

}

void AReclusionPlayerController::SetupInputComponent() {

	Super::SetupInputComponent();

	if (PlayerInput) {

		//PlayerInput->SetMouseSensitivityToDefault();

	}

}

void AReclusionPlayerController::PostInitializeComponents() {

	Super::PostInitializeComponents();

}

void AReclusionPlayerController::BeginPlay() {

	Super::BeginPlay();

}

void AReclusionPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) {

	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

}