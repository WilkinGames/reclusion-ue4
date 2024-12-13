// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "ReclusionAIController.generated.h"

/**
 * AI controller class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionAIController : public AAIController {

	GENERATED_UCLASS_BODY()

protected:

	virtual void PostInitializeComponents() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
};