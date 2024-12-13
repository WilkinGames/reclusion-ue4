// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionMutantCharacter.h"
#include "ReclusionCharacterMovement.h"

AReclusionMutantCharacter::AReclusionMutantCharacter(const FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UReclusionCharacterMovement>(ACharacter::CharacterMovementComponentName)) {

	GetCapsuleComponent()->bReceivesDecals = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}

void AReclusionMutantCharacter::PostInitializeComponents() {

	Super::PostInitializeComponents();

	Health = HealthConfig.HealthMax;

}

void AReclusionMutantCharacter::Destroyed() {

	Super::Destroyed();

}

void AReclusionMutantCharacter::Tick(float DeltaSeconds) {

	Super::Tick(DeltaSeconds);

}

void AReclusionMutantCharacter::SetRagdollPhysics() {

	bool bInRagdoll = false;

	if (IsPendingKill()) {

		bInRagdoll = false;

	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset()) {

		bInRagdoll = false;

	}
	else {

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;

	}

	GetRootComponent()->DestroyComponent();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll) {

		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);

	}
	else {

		SetLifeSpan(10.0f);

	}

}

bool AReclusionMutantCharacter::IsAlive() const {

	return (Health > 0.0f);

}

float AReclusionMutantCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {

	if (Health <= 0.0f) {

		return 0.0f;

	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f) {

		Health -= ActualDamage;

		if (Health <= 0.0f) {

			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

		}
		else {

			//Play hit

		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);

	}

	return ActualDamage;

}

bool AReclusionMutantCharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser) {

	Health = FMath::Min(Health, 0.0f);

	SetRagdollPhysics();

	return true;

}