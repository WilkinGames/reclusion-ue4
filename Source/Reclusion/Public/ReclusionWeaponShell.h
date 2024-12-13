// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ReclusionWeaponShell.generated.h"

/**
 * Ejected weapon shell class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionWeaponShell : public AActor {

	GENERATED_UCLASS_BODY()	

private:

	virtual void PostInitializeComponents() override;

protected:

	/*

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* MeshShell;

	*/

public:

	/*

	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category=Movement)
	void SetVelocity(FVector NewVelocity);

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; };

	*/
	
};