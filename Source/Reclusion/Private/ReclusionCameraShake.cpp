// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionCameraShake.h"

UReclusionCameraShake::UReclusionCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	bSingleInstance = true;

	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;

	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;

}