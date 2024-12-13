// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionWeaponShell.h"

AReclusionWeaponShell::AReclusionWeaponShell(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	/*

	MeshShell = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponShellMesh"));
	MeshShell->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	MeshShell->bReceivesDecals = false;
	MeshShell->SetSimulatePhysics(true);
	MeshShell->BodyInstance.SetCollisionProfileName("WeaponProfile");
	MeshShell->OnComponentHit.AddDynamic(this, &AReclusionWeaponShell::OnHit);
	MeshShell->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	MeshShell->CanCharacterStepUpOn = ECB_No;

	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = MeshShell;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->bShouldBounce = true;

	RootComponent = MeshShell;

	*/

	bCanBeDamaged = false;
	InitialLifeSpan = 10.0f;

}

void AReclusionWeaponShell::PostInitializeComponents() {

	Super::PostInitializeComponents();

}
/*

void AReclusionWeaponShell::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (GEngine) {

		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "SHELL HIT");

	}

}

void AReclusionWeaponShell::SetVelocity(FVector NewVelocity) {

	if (ProjectileMovement) {

		ProjectileMovement->Velocity = NewVelocity;

	}

}

*/