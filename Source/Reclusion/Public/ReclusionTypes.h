// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "ReclusionTypes.generated.h"

/* Keep this in sync with ReclusionImpactEffect. */
UENUM()
namespace EReclusionPhysMaterialType {

	enum Type {

		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,

	};

}

#define RECLUSION_SURFACE_Default		SurfaceType_Default
#define RECLUSION_SURFACE_Concrete		SurfaceType1
#define RECLUSION_SURFACE_Dirt			SurfaceType2
#define RECLUSION_SURFACE_Water			SurfaceType3
#define RECLUSION_SURFACE_Metal			SurfaceType4
#define RECLUSION_SURFACE_Wood			SurfaceType5
#define RECLUSION_SURFACE_Grass			SurfaceType6
#define RECLUSION_SURFACE_Glass			SurfaceType7
#define RECLUSION_SURFACE_Flesh			SurfaceType8