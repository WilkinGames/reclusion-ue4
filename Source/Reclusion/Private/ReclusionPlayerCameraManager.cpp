// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionPlayerCameraManager.h"
#include "ReclusionCharacter.h"
#include "ReclusionWeapon.h"
#include "ReclusionWeaponAttachment.h"
#include "ReclusionCharacterMovement.h"

AReclusionPlayerCameraManager::AReclusionPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	WeaponLocationDelta = FVector(0.0f, 0.0f, 0.0f);
	WeaponRotationDelta = FRotator(0.0f, 0.0f, 0.0f);
	WeaponBaseLocation = FVector(0.0f, 0.0f, 0.0f);

	bPendingWeaponKick = false;

	IdleFieldOfView = 90.0f;
	RunningFieldOfViewMultiplier = 1.0f;

	bEnableWeaponSway = true;
	WeaponSwayMultiplier = 0.02;
	WeaponSwayRotationMultiplier = 2.0f;
	WeaponSwayInterpAlpha = 0.02f;

	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;

	bAlwaysApplyModifiers = true;

}

void AReclusionPlayerCameraManager::PostInitializeComponents() {

	Super::PostInitializeComponents();

}

void AReclusionPlayerCameraManager::UpdateCamera(float DeltaTime) {

	FRotator RotOld;
	FRotator RotNew;
	FVector LocOld;
	FVector LocNew;

	AReclusionCharacter* MyPawn = PCOwner ? Cast<AReclusionCharacter>(PCOwner->GetPawn()) : NULL;

	if (MyPawn) {

		float RunningFieldOfView = (RunningFieldOfViewMultiplier * IdleFieldOfView);
		float WeaponAimingFieldOfView = IdleFieldOfView;
		float TargetFieldOfView;

		AReclusionWeapon* Weapon = MyPawn->GetWeapon();

		if (Weapon && Weapon->GetAttachmentOptic()) {

			FAttachmentModifierData OpticModifier = Weapon->GetAttachmentOptic()->GetModifierConfig();
			WeaponAimingFieldOfView = (OpticModifier.AimingFieldOfViewMultiplier * IdleFieldOfView);

		}
		
		TargetFieldOfView = MyPawn->IsAiming() ? WeaponAimingFieldOfView : IdleFieldOfView;
		TargetFieldOfView = MyPawn->IsRunning() ? RunningFieldOfView : TargetFieldOfView;

		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFieldOfView, DeltaTime, 20.0f);

		SetFOV(DefaultFOV);

		RotOld = GetCameraRotation();
		LocOld = GetCameraLocation();

	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn) {

		RotNew = GetCameraRotation();
		LocNew = GetCameraLocation();

		if (bEnableWeaponSway) {

			float LocationDeltaX = 0.0f;
			float LocationDeltaY = 0.0f;
			float LocationDeltaZ = (RotOld.Pitch - RotNew.Pitch);

			if ((RotNew.Yaw > 0 && RotOld.Yaw < 0) && RotNew.Yaw > 90) {

				LocationDeltaY = (RotNew.Yaw + RotOld.Yaw);

			}
			else if ((RotNew.Yaw < 0 && RotOld.Yaw > 0) && RotNew.Yaw < -90) {

				LocationDeltaY = (RotOld.Yaw + RotNew.Yaw);

			}
			else {

				LocationDeltaY = (RotOld.Yaw - RotNew.Yaw);

			}

			FVector Velocity = (-MyPawn->GetActorRotation()).RotateVector(MyPawn->GetVelocity());
			float VelocityMultiplier = MyPawn->IsAiming() ? 0.0012f : 0.005f;

			LocationDeltaX += -Velocity.X * VelocityMultiplier;
			LocationDeltaY += -Velocity.Y * VelocityMultiplier;
			LocationDeltaZ += (Velocity.Z - Velocity.X) * VelocityMultiplier;

			if (!MyPawn->IsAiming()) {

				LocationDeltaX += RotNew.Pitch * 0.05f;
				LocationDeltaZ += RotNew.Pitch * 0.05f;

				if (MyPawn->IsCrouching()) {

					LocationDeltaZ += 3.0f;

				}

				if (MyPawn->GetCharacterMovement()->IsFalling()) {

					LocationDeltaZ += 6.0f;

				}

			}

			if (bPendingWeaponKick) {

				LocationDeltaX += WeaponKick.X;
				LocationDeltaY += WeaponKick.Y;
				LocationDeltaZ += WeaponKick.Z;

				bPendingWeaponKick = false;

				WeaponKick = FVector::ZeroVector;

			}

			WeaponLocationDelta.X = LocationDeltaX * WeaponSwayMultiplier;
			WeaponLocationDelta.Y = LocationDeltaY * WeaponSwayMultiplier;
			WeaponLocationDelta.Z = LocationDeltaZ * WeaponSwayMultiplier;
			WeaponLocationDelta += MyPawn->GetMesh1P()->GetRelativeTransform().GetLocation();			

			WeaponLocationDelta.X = FMath::InterpExpoOut(WeaponLocationDelta.X, WeaponBaseLocation.X, WeaponSwayInterpAlpha);
			WeaponLocationDelta.Y = FMath::InterpExpoOut(WeaponLocationDelta.Y, WeaponBaseLocation.Y, WeaponSwayInterpAlpha);
			WeaponLocationDelta.Z = FMath::InterpExpoOut(WeaponLocationDelta.Z, WeaponBaseLocation.Z, WeaponSwayInterpAlpha);

			float RotationDeltaPitch = (WeaponLocationDelta.Y + WeaponKick.Z) * WeaponSwayRotationMultiplier;
			float RotationDeltaYaw = -90.0f;
			float RotationDeltaRoll = WeaponLocationDelta.Z * WeaponSwayRotationMultiplier;

			WeaponRotationDelta.Pitch = RotationDeltaPitch;
			WeaponRotationDelta.Yaw = RotationDeltaYaw;
			WeaponRotationDelta.Roll = RotationDeltaRoll;

			MyPawn->GetMesh1P()->SetRelativeLocation(WeaponLocationDelta);
			MyPawn->GetMesh1P()->SetRelativeRotation(WeaponRotationDelta);

		}

		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());

	}

}

void AReclusionPlayerCameraManager::AddWeaponKick(FVector NewWeaponKick) {

	bPendingWeaponKick = true;

	WeaponKick = NewWeaponKick;

}