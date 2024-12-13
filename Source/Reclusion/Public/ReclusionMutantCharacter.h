// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "ReclusionMutantCharacter.generated.h"

USTRUCT()
struct FMutantHealthData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Health)
	float HealthMax;

	FMutantHealthData() {

		HealthMax = 100.0f;

	}

};

/**
 * Mutant character class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionMutantCharacter : public ACharacter {

	GENERATED_UCLASS_BODY()

private:

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FMutantHealthData HealthConfig;

	UPROPERTY(VisibleDefaultsOnly, Category=Pawn)
	float Health;

	void SetRagdollPhysics();

	UFUNCTION(BlueprintCallable, Category=Pawn)
	bool IsAlive() const;

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

};