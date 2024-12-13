// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "ReclusionHUD.generated.h"

/**
 * HUD class.
 */
UCLASS()
class AReclusionHUD : public AHUD {

	GENERATED_BODY()

public:

	AReclusionHUD(const FObjectInitializer& ObjectInitializer);

	virtual void DrawHUD() override;

};