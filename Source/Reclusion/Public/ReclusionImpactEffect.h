// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ReclusionTypes.h"
#include "ReclusionImpactEffect.generated.h"

USTRUCT()
struct FDecalData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	UMaterial* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float DecalSize;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float LifeSpan;

	FDecalData() {

		DecalSize = 32.0f;
		LifeSpan = 10.0f;

	}

};

USTRUCT()
struct FImpactEffectData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Impact)
	UParticleSystem* ImpactFX;

	UPROPERTY(EditDefaultsOnly, Category=Impact)
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category=Impact)
	struct FDecalData ImpactDecalConfig;

};

/**
 * Weapon impact effect class.
 */
UCLASS(Abstract, Blueprintable)
class RECLUSION_API AReclusionImpactEffect : public AActor {

	GENERATED_UCLASS_BODY()	

	UPROPERTY(EditDefaultsOnly, Category=Default)
	struct FImpactEffectData ImpactDefaultConfig;

	UPROPERTY(EditDefaultsOnly, Category=Concrete)
	struct FImpactEffectData ImpactConcreteConfig;

	UPROPERTY(EditDefaultsOnly, Category=Dirt)
	struct FImpactEffectData ImpactDirtConfig;

	UPROPERTY(EditDefaultsOnly, Category=Water)
	struct FImpactEffectData ImpactWaterConfig;

	UPROPERTY(EditDefaultsOnly, Category=Metal)
	struct FImpactEffectData ImpactMetalConfig;

	UPROPERTY(EditDefaultsOnly, Category=Wood)
	struct FImpactEffectData ImpactWoodConfig;

	UPROPERTY(EditDefaultsOnly, Category=Grass)
	struct FImpactEffectData ImpactGrassConfig;

	UPROPERTY(EditDefaultsOnly, Category=Glass)
	struct FImpactEffectData ImpactGlassConfig;

	UPROPERTY(EditDefaultsOnly, Category=Flesh)
	struct FImpactEffectData ImpactFleshConfig;

	UPROPERTY(BlueprintReadOnly, Category=Surface)
	FHitResult SurfaceHit;

	virtual void PostInitializeComponents() override;

protected:

	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	FDecalData GetDecalConfig(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

};