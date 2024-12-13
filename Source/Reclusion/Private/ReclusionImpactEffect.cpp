// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionImpactEffect.h"

AReclusionImpactEffect::AReclusionImpactEffect(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) {

	bAutoDestroyWhenFinished = true;

}

void AReclusionImpactEffect::PostInitializeComponents() {

	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);

	if (ImpactFX) {

		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());

	}

	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);

	if (ImpactSound) {

		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	}

	FDecalData DecalConfig = GetDecalConfig(HitSurfaceType);

	if (DecalConfig.DecalMaterial) {

		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DecalConfig.DecalMaterial, FVector(DecalConfig.DecalSize, DecalConfig.DecalSize, 1.0f),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DecalConfig.LifeSpan);

	}

}

UParticleSystem* AReclusionImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const {

	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType) {

		case RECLUSION_SURFACE_Concrete:
			ImpactFX = ImpactConcreteConfig.ImpactFX;
			break;
		case RECLUSION_SURFACE_Dirt:		
			ImpactFX = ImpactDirtConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Water:		
			ImpactFX = ImpactWaterConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Metal:		
			ImpactFX = ImpactMetalConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Wood:		
			ImpactFX = ImpactWoodConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Grass:		
			ImpactFX = ImpactGrassConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Glass:		
			ImpactFX = ImpactGlassConfig.ImpactFX; 
			break;
		case RECLUSION_SURFACE_Flesh:		
			ImpactFX = ImpactFleshConfig.ImpactFX; 
			break;
		default:
			ImpactFX = ImpactDefaultConfig.ImpactFX; 
			break;

	}

	return ImpactFX;

}

USoundCue* AReclusionImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const {

	USoundCue* ImpactSound = NULL;

	switch (SurfaceType) {

	case RECLUSION_SURFACE_Concrete:
		ImpactSound = ImpactConcreteConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Dirt:
		ImpactSound = ImpactDirtConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Water:
		ImpactSound = ImpactWaterConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Metal:
		ImpactSound = ImpactMetalConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Wood:
		ImpactSound = ImpactWoodConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Grass:
		ImpactSound = ImpactGrassConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Glass:
		ImpactSound = ImpactGlassConfig.ImpactSound;
		break;
	case RECLUSION_SURFACE_Flesh:
		ImpactSound = ImpactFleshConfig.ImpactSound;
		break;
	default:
		ImpactSound = ImpactDefaultConfig.ImpactSound;
		break;

	}

	return ImpactSound;

}

FDecalData AReclusionImpactEffect::GetDecalConfig(TEnumAsByte<EPhysicalSurface> SurfaceType) const {

	FDecalData Decal;

	switch (SurfaceType) {

	case RECLUSION_SURFACE_Concrete:
		Decal = ImpactConcreteConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Dirt:
		Decal = ImpactDirtConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Water:
		Decal = ImpactWaterConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Metal:
		Decal = ImpactMetalConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Wood:
		Decal = ImpactWoodConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Grass:
		Decal = ImpactGrassConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Glass:
		Decal = ImpactGlassConfig.ImpactDecalConfig;
		break;
	case RECLUSION_SURFACE_Flesh:
		Decal = ImpactFleshConfig.ImpactDecalConfig;
		break;
	default:
		Decal = ImpactDefaultConfig.ImpactDecalConfig;
		break;

	}

	return Decal;

}