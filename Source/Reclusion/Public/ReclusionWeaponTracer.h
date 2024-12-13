// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ReclusionWeaponTracer.generated.h"

/**
 * Weapon bullet tracer class.
 */
UCLASS(config=Game)
class RECLUSION_API AReclusionWeaponTracer : public AActor {
	
	GENERATED_BODY()

private:

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category=Movement)
	class UProjectileMovementComponent* ProjectileMovement;

public:

	AReclusionWeaponTracer(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
};