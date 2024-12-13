// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionAIController.h"

AReclusionAIController::AReclusionAIController(const FObjectInitializer& PCIP)
	: Super(PCIP) {

	//...

}

void AReclusionAIController::PostInitializeComponents() {

	Super::PostInitializeComponents();

}

void AReclusionAIController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) {

	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

}