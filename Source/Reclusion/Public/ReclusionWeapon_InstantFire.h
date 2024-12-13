// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "ReclusionWeapon.h"
#include "ReclusionWeapon_InstantFire.generated.h"

USTRUCT()
struct FInstantFireWeaponData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Debug)
	bool DrawDebugTrace;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float AimingSpreadMod;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float CrouchingSpreadMod;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float FallingSpreadMod;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float FiringSpreadIncrement;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float FiringSpreadMax;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	int32 HitDamage;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float HitDamageHeadshotMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	float WeaponTracerChance;

	FInstantFireWeaponData() {

		DrawDebugTrace = false;
		WeaponSpread = 5.0f;
		AimingSpreadMod = 0.01f;
		CrouchingSpreadMod = 0.5f;
		FallingSpreadMod = 1.5f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		HitDamageHeadshotMultiplier = 1.5f;
		DamageType = UDamageType::StaticClass();
		ClientSideHitLeeway = 200.0f;
		AllowedViewDotHitDir = 0.8f;
		WeaponTracerChance = 0.5f;

	}

};

USTRUCT()
struct FInstantHitInfo {

	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;

};

/**
 * Instant fire weapon class.
 */
UCLASS()
class RECLUSION_API AReclusionWeapon_InstantFire : public AReclusionWeapon {

	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FInstantFireWeaponData InstantFireConfig;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class AReclusionImpactEffect> ImpactTemplate;

	float CurrentFiringSpread;

	FVector LastEndTrace;

	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	virtual void FireWeapon() override;

	virtual void SimulateWeaponFire() override;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	bool ShouldDealDamage(AActor* TestActor) const;

	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	void SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread);

	void SpawnImpactEffects(const FHitResult& Impact);

	UFUNCTION()
	void OnRep_HitNotify();

public:

	float GetCurrentSpread() const;
	
};