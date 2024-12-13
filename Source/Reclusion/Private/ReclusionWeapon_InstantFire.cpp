// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionWeapon_InstantFire.h"
#include "ReclusionImpactEffect.h"
#include "ReclusionPlayerController.h"
#include "ReclusionPlayerCameraManager.h"

void AReclusionWeapon_InstantFire::FireWeapon() {

	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);
	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantFireConfig.WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
	ApplyRecoil();	

	CurrentFiringSpread = FMath::Min(InstantFireConfig.FiringSpreadMax, CurrentFiringSpread + InstantFireConfig.FiringSpreadIncrement);
	LastEndTrace = EndTrace;

	if (MyPawn) {

		AReclusionPlayerController* PC = Cast<AReclusionPlayerController>(MyPawn->Controller);
		AReclusionPlayerCameraManager* CM = Cast<AReclusionPlayerCameraManager>(PC->PlayerCameraManager);

		if (CM) {

			CM->AddWeaponKick(FVector(-30.0f, 0.0f, 2.0f));

		}

	}

}

void AReclusionWeapon_InstantFire::SimulateWeaponFire() {

	Super::SimulateWeaponFire();

	if (WeaponTracer && InstantFireConfig.WeaponTracerChance > 0) {

		float Rand = FMath::FRandRange(0, 1);

		if (Rand <= InstantFireConfig.WeaponTracerChance) {

			const FRotator SpawnRotation = MyPawn->GetControlRotation();
			const FVector SpawnLocation = GetMuzzleLocation();

			FVector Angle = (LastEndTrace - SpawnLocation);
			FRotator Rotation = FRotationMatrix::MakeFromX(Angle).Rotator();

			UWorld* const World = GetWorld();

			if (World != NULL) {

				World->SpawnActor<AReclusionWeaponTracer>(WeaponTracer, SpawnLocation, Rotation);

			}

		}

	}

}

FHitResult AReclusionWeapon_InstantFire::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const {

	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	
	if (InstantFireConfig.DrawDebugTrace == true) {

		GetWorld()->DebugDrawTraceTag = WeaponFireTag;

	}

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;

}

void AReclusionWeapon_InstantFire::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread) {

	if (MyPawn && MyPawn->IsLocallyControlled() /* && GetNetMode() == NM_Client */) {

		if (Impact.GetActor() /* && Impact.GetActor()->GetRemoteRole() == ROLE_Authority */) {

			ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);

		}
		else if (Impact.GetActor() == NULL) {

			if (Impact.bBlockingHit) {

				ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);

			}
			else {

				ServerNotifyMiss(ShootDir, RandomSeed, ReticleSpread);

			}

		}

	}

	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);

}

void AReclusionWeapon_InstantFire::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread) {

	if (ShouldDealDamage(Impact.GetActor())) {

		DealDamage(Impact, ShootDir);

	}

	if (Role == ROLE_Authority) {

		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;

	}

	if (GetNetMode() != NM_DedicatedServer) {

		const FVector EndTrace = Origin + ShootDir * InstantFireConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		SpawnImpactEffects(Impact);

	}

}

bool AReclusionWeapon_InstantFire::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) {
	
	return true;

}

void AReclusionWeapon_InstantFire::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) {

	return;

}

bool AReclusionWeapon_InstantFire::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) {

	return true;

}

void AReclusionWeapon_InstantFire::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) {

	return;

}

bool AReclusionWeapon_InstantFire::ShouldDealDamage(AActor* TestActor) const {

	if (TestActor) {

		if (GetNetMode() != NM_Client || TestActor->Role == ROLE_Authority || TestActor->bTearOff) {

			return true;

		}

	}

	return false;

}

void AReclusionWeapon_InstantFire::DealDamage(const FHitResult& Impact, const FVector& ShootDir) {

	FPointDamageEvent PointDmg;

	PointDmg.DamageTypeClass = InstantFireConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantFireConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);

}

float AReclusionWeapon_InstantFire::GetCurrentSpread() const {

	float FinalSpread = InstantFireConfig.WeaponSpread + CurrentFiringSpread;

	if (MyPawn) {

		if (MyPawn->IsAiming()) {

			FinalSpread *= InstantFireConfig.AimingSpreadMod;

		}

		if (MyPawn->IsCrouching()) {

			FinalSpread *= InstantFireConfig.CrouchingSpreadMod;

		}

		if (MyPawn->GetCharacterMovement()->IsFalling()) {

			FinalSpread *= InstantFireConfig.FallingSpreadMod;

		}

	}

	return FinalSpread;

}

void AReclusionWeapon_InstantFire::SpawnImpactEffects(const FHitResult& Impact) {

	if (ImpactTemplate && Impact.bBlockingHit) {

		bool SpawnImpactEffect = true;
		FHitResult UseImpact = Impact;

		if (!Impact.Component.IsValid()) {

			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;

		}

		AReclusionImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AReclusionImpactEffect>(ImpactTemplate, Impact.ImpactPoint, Impact.ImpactNormal.Rotation());

		if (EffectActor) {

			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));

		}

	}

}

void AReclusionWeapon_InstantFire::OnRep_HitNotify() {

	SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread);

}

void AReclusionWeapon_InstantFire::SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread) {

	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread * 0.5f);
	const FVector StartTrace = ShotOrigin;
	const FVector AimDir = GetAdjustedAim();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantFireConfig.WeaponRange;

	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	if (Impact.bBlockingHit) {

		SpawnImpactEffects(Impact);

	}

}

void AReclusionWeapon_InstantFire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(AReclusionWeapon_InstantFire, HitNotify, COND_SkipOwner);

}