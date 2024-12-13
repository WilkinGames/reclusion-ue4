// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionCharacter.h"
#include "ReclusionWeapon.h"
#include "ReclusionWeaponAttachment.h"
#include "ReclusionPlayerController.h"
#include "ReclusionCharacterMovement.h"
#include "ReclusionCameraShake.h"
#include "ReclusionPlayerCameraManager.h"
#include "Animation/AnimInstance.h"

AReclusionCharacter::AReclusionCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UReclusionCharacterMovement>(ACharacter::CharacterMovementComponentName)) {

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	DecBEH = BaseEyeHeight;
	DecCapsHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	AimingSpeedModifier = 0.5f;	
	RunningSpeedModifier = 2.0f;
	CrouchingSpeedModifier = 0.5f;
	CrouchingRecoilModifier = 0.5f;

	bIsCrouching = false;
	bIsAiming = false;
	bWantsToRun = false;
	bWantsToFire = false;

	StepCounter = 0.0f;
	BaseStepSpeed = 50.0f;

	DepthOfFieldDistance = 0.0f;

	RecoilPitch = 0.0f;
	RecoilPitchRecovery = 0.0f;
	RecoilYaw = 0.0f;
	RecoilYawRecovery = 0.0f;

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	BaseAimRate = 100.0f;

}

void AReclusionCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &AReclusionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AReclusionCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AReclusionCharacter::LookUpRate);
	InputComponent->BindAxis("LookUpRate", this, &AReclusionCharacter::LookUpAtRate);
	InputComponent->BindAxis("Turn", this, &AReclusionCharacter::TurnRate);
	InputComponent->BindAxis("TurnRate", this, &AReclusionCharacter::TurnAtRate);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AReclusionCharacter::OnStartCrouching);
	InputComponent->BindAction("Crouch", IE_Released, this, &AReclusionCharacter::OnStopCrouching);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AReclusionCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);	
	InputComponent->BindAction("Fire", IE_Pressed, this, &AReclusionCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AReclusionCharacter::OnStopFire);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AReclusionCharacter::OnStartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &AReclusionCharacter::OnStopAiming);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AReclusionCharacter::OnReload);
	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AReclusionCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AReclusionCharacter::OnPrevWeapon);	
	InputComponent->BindAction("Run", IE_Pressed, this, &AReclusionCharacter::OnStartRunning);
	InputComponent->BindAction("RunToggle", IE_Pressed, this, &AReclusionCharacter::OnStartRunningToggle);
	InputComponent->BindAction("Run", IE_Released, this, &AReclusionCharacter::OnStopRunning);

}

void AReclusionCharacter::PostInitializeComponents() {
	
	Super::PostInitializeComponents();

	Health = HealthConfig.HealthMax;
	Stamina = StaminaConfig.StaminaMax;

	SpawnDefaultInventory();

}

void AReclusionCharacter::Tick(float DeltaSeconds) {

	Super::Tick(DeltaSeconds);
	
	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		UReclusionCharacterMovement* CM = Cast<UReclusionCharacterMovement>(GetCharacterMovement());

		/* Manage weapon recoil */

		if (GetWeapon()) {

			FRecoilData RecoilConfig = GetWeapon()->GetRecoilConfig();

			RecoilPitch = FMath::InterpExpoOut(RecoilPitch, 0.0f, RecoilConfig.InterpKickSpeed);
			RecoilPitchRecovery = FMath::InterpExpoOut(RecoilPitchRecovery, -RecoilPitch, RecoilConfig.InterpRecoverySpeed);
			float ApplyPitch = (RecoilPitch + RecoilPitchRecovery) * (DeltaSeconds * 100.0f);

			RecoilYaw = FMath::InterpExpoOut(RecoilYaw, 0.0f, RecoilConfig.InterpKickSpeed);
			RecoilYawRecovery = FMath::InterpExpoOut(RecoilYawRecovery, -RecoilYaw, RecoilConfig.InterpRecoverySpeed);
			float ApplyYaw = (RecoilYaw + RecoilYawRecovery) * (DeltaSeconds * 100.0f);

			MyPC->AddPitchInput(ApplyPitch);
			MyPC->AddYawInput(ApplyYaw);

			if (RecoilConfig.AddYawAsRoll) {

				MyPC->AddRollInput(ApplyYaw);

			}

			/* DOF trace */

			static FName DOFTag = FName(TEXT("DOFTrace"));
			const int32 RandomSeed = FMath::Rand();
			FRandomStream WeaponRandomStream(RandomSeed);
			const float ConeHalfAngle = FMath::DegreesToRadians(0.0f);
			const FVector AimDir = GetWeapon()->GetAdjustedAim();
			const FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
			const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
			const FVector EndTrace = StartTrace + ShootDir * 10000.0f;
			FCollisionQueryParams TraceParams(DOFTag, true, Instigator);
			TraceParams.bTraceAsyncScene = true;
			FHitResult Hit(ForceInit);
			//GetWorld()->DebugDrawTraceTag = DOFTag;
			bool bHit = GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
			float Dist = bHit ? FVector::Dist(StartTrace, Hit.Location) : 5000.0f;

			DepthOfFieldDistance = FMath::FInterpTo(DepthOfFieldDistance, Dist, DeltaSeconds, 10.0f);

			/* DOF method (DOFM_Gaussian = enabled, DOFM_BokehDOF = disabled) */

			EDepthOfFieldMethod DepthOfFieldMethod = IsAiming() ? EDepthOfFieldMethod::DOFM_Gaussian : EDepthOfFieldMethod::DOFM_BokehDOF;

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldMethod = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldMethod = DepthOfFieldMethod;

			/* DOF focal distance */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldFocalDistance = DepthOfFieldDistance;

			/* DOF focal region */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldFocalRegion = 1000.0f;

			/* DOF far transition region */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldFarTransitionRegion = 5000.0f;

			/* DOF near transition region */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldNearTransitionRegion = DepthOfFieldDistance * 5.0f;

			/* DOF near blur size */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldNearBlurSize = 0.4f;

			/* DOF far blur size */

			GetFirstPersonCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = true;
			GetFirstPersonCameraComponent()->PostProcessSettings.DepthOfFieldFarBlurSize = bHit ? 1.0f : 0.0f;

		}

		/* Manage player status (stamina, movement, camera shakes) */

		float StaminaDelta = (Stamina / StaminaConfig.StaminaMax);

		if (IsRunning()) {

			BaseStepSpeed = 30.0f;

			Stamina += StaminaConfig.StaminaRunningDrainRate * (DeltaSeconds * 100.0f);
			Stamina = FMath::Max<float>(Stamina, StaminaConfig.StaminaMin);

			if (Stamina <= StaminaConfig.StaminaMin) {

				SetRunning(false, false);

			}

		}
		else {

			if (IsCrouching()) {

				BaseStepSpeed = 60.0f;

			}
			else {

				BaseStepSpeed = 50.0f;

			}

			if (Stamina < StaminaConfig.StaminaMax && !CM->IsFalling()) {

				//float LastStamina = Stamina;

				float MovementMultiplier = 1.0f - (GetVelocity().Size() / (CM->MaxWalkSpeed * 2.5f)) * (CM->IsFalling() ? 0.5f : 1.0f);
				float RecoveryMultiplier = FMath::Max<float>(StaminaDelta, 0.2f) * 1.5f;

				Stamina += (StaminaConfig.StaminaBaseRecoveryRate * MovementMultiplier * RecoveryMultiplier * (IsCrouching() ? StaminaConfig.StaminaCrouchingRecoveryMultiplier : 1.0f)) * (DeltaSeconds * 100.0f);
				
				Stamina = FMath::Min<float>(Stamina, StaminaConfig.StaminaMax);

			}

		}

		bool bIsMovingOnGround = (GetCharacterMovement()->IsMovingOnGround() && GetVelocity().Size() > 1.0f);

		if (bIsMovingOnGround) {

			if (IsCrouching()) {

				MovementCameraShakeClass = CameraShakeConfig.MovementCrouching;

			}
			else {

				if (IsRunning()) {

					MovementCameraShakeClass = CameraShakeConfig.MovementRunning;

				}
				else {

					MovementCameraShakeClass = CameraShakeConfig.MovementWalking;

				}

			}

		}
		else {

			MovementCameraShakeClass = CameraShakeConfig.Idle;

		}

		float CamShakeScale = 1.0f;

		if (IsAiming()) {

			CamShakeScale = GetAimingSpeedModifier() * 0.5f;

		}

		if (MovementCameraShakeClass) {

			MyPC->ClientPlayCameraShake(MovementCameraShakeClass, CamShakeScale);

		}

		if (GetVelocity().Size() > 1.0f && !GetCharacterMovement()->IsFalling()) {

			StepCounter -= (DeltaSeconds * 100.0f);

			if (StepCounter < 0) {

				StepCounter = BaseStepSpeed;

				PlaySoundOnActor(FootstepConfig.Generic);

			}

		}
		else {

			StepCounter = BaseStepSpeed * 0.5f;

		}

		if (RunLoopAudioComponent) {

			float RunLoopMultiplier = 0.75f - StaminaDelta;
			float RunLoopPitchMultiplier = FMath::Max<float>(FMath::Min<float>(0.5f + RunLoopMultiplier, 1.05f), 0.95f);

			RunLoopAudioComponent->SetVolumeMultiplier(RunLoopMultiplier);
			RunLoopAudioComponent->SetPitchMultiplier(RunLoopPitchMultiplier);

			if (RunLoopMultiplier > 0.0f && MovementCameraShakeClass == CameraShakeConfig.Idle) {

				RunLoopMultiplier = RunLoopMultiplier * (IsAiming() ? 0.5f : 1.0f);

				MyPC->ClientPlayCameraShake(CameraShakeConfig.IdleLowStamina, RunLoopMultiplier);

			}

		}

		/* Manage crosshair */

		if (!IsAiming()) {

			float ActualCrosshairSpread = FVector(GetVelocity().X, GetVelocity().Y, 0.0f).Size();

			ActualCrosshairSpread += GetCurrentRecoil() + (IsCrouching() ? 0.0f : 200.0f) + (CM->IsFalling() ? 600.0f : 0.0f);
			ActualCrosshairSpread = FMath::GetMappedRangeValue(FVector2D(0.0f, 1200.0f), FVector2D(20.0f, 120.0f), ActualCrosshairSpread);

			CrosshairSpread = FMath::FInterpTo(CrosshairSpread, ActualCrosshairSpread, DeltaSeconds, 20.0f);

		}

		/* Manage crouching */

		CrouchTick(DeltaSeconds);

	}

	/* Manage debugging */

	if (GEngine) {

		//GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, "Health: " + FString::SanitizeFloat(Health));
		//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Cyan, "Stamina: " + FString::SanitizeFloat(Stamina));

	}

}

void AReclusionCharacter::Destroyed() {

	Super::Destroyed();

}

void AReclusionCharacter::OnLanded(const FHitResult& Hit) {

	Super::OnLanded(Hit);

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		float JumpLandThreshold = 700.0f;
		float JumpLandScale = FMath::Abs(GetVelocity().Z) / JumpLandThreshold;

		JumpLandScale = FMath::Min<float>(JumpLandScale, 1.0f);

		PlaySoundOnActor(JumpLandSound, JumpLandScale);

		MyPC->ClientPlayCameraShake(CameraShakeConfig.JumpLand, JumpLandScale);

	}

}

float AReclusionCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

}

void AReclusionCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) {

	//...

}

void AReclusionCharacter::SpawnDefaultInventory() {

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();

	for (int32 i = 0; i < NumWeaponClasses; i ++) {

		if (DefaultInventoryClasses[i]) {

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;

			AReclusionWeapon* NewWeapon = GetWorld()->SpawnActor<AReclusionWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			
			AddWeapon(NewWeapon);

		}

	}

	if (Inventory.Num() > 0) {

		EquipWeapon(Inventory[0]);

	}

}

void AReclusionCharacter::DestroyInventory() {

	for (int32 i = Inventory.Num() - 1; i >= 0; i--) {

		AReclusionWeapon* Weapon = Inventory[i];

		if (Weapon) {

			RemoveWeapon(Weapon);
			
			Weapon->Destroy();

		}

	}

}

void AReclusionCharacter::SetCurrentWeapon(class AReclusionWeapon* NewWeapon, class AReclusionWeapon* LastWeapon) {

	AReclusionWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL) {

		LocalLastWeapon = LastWeapon;

	}
	else if (NewWeapon != CurrentWeapon) {

		LocalLastWeapon = CurrentWeapon;

	}

	if (LocalLastWeapon) {

		LocalLastWeapon->OnUnEquip();

	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon) {

		NewWeapon->SetOwningPawn(this);		
		NewWeapon->OnEquip();

	}

}

void AReclusionCharacter::AddWeapon(class AReclusionWeapon* Weapon) {

	Weapon->OnEnterInventory(this);

	Inventory.AddUnique(Weapon);

}

void AReclusionCharacter::RemoveWeapon(class AReclusionWeapon* Weapon) {

	Weapon->OnLeaveInventory();

	Inventory.RemoveSingle(Weapon);

}

void AReclusionCharacter::EquipWeapon(class AReclusionWeapon* Weapon) {

	if (Weapon) {

		SetCurrentWeapon(Weapon);

	}

}

AReclusionWeapon* AReclusionCharacter::GetWeapon() const {

	return CurrentWeapon;

}

void AReclusionCharacter::OnStartCrouching() {

	SetCrouching(true);

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (IsRunning()) {

			SetRunning(false, false);

		}

	}

}

void AReclusionCharacter::OnStopCrouching() {

	SetCrouching(false);

}

void AReclusionCharacter::CrouchTick(float DeltaTime) {

	const float TargetBEH = bIsCrouching ? CrouchedEyeHeight : DecBEH;
	const float TargetCapsuleSize = bIsCrouching ? GetCharacterMovement()->CrouchedHalfHeight : DecCapsHeight;

	if (Controller != NULL) {

		//BaseEyeHeight = FMath::FInterpTo(BaseEyeHeight, TargetBEH, DeltaTime, 10.0f);
		
		BaseEyeHeight = FMath::InterpExpoOut(BaseEyeHeight, TargetBEH, 0.1f);

		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::FInterpTo(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), TargetCapsuleSize, DeltaTime, 10.0f), true);
		
		const float Dist = TargetCapsuleSize - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float DeltaMovCaps = Dist * FMath::Clamp<float>(0.0f, 0.0f, 1.0f);

		GetCapsuleComponent()->SetRelativeLocation(FVector(GetCapsuleComponent()->RelativeLocation.X, GetCapsuleComponent()->RelativeLocation.Y, (GetCapsuleComponent()->RelativeLocation.Z + DeltaMovCaps)), true);
	
	}

}

void AReclusionCharacter::OnStartFire() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (IsRunning()) {

			SetRunning(false, false);

		}
		else {

			StartWeaponFire();

		}

	}

}

void AReclusionCharacter::OnStopFire() {

	StopWeaponFire();

}

void AReclusionCharacter::OnReload() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (CurrentWeapon) {

			CurrentWeapon->StartReload();

		}

	}

}

void AReclusionCharacter::OnStartAiming() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC && GetWeapon()) {

		if (IsRunning()) {

			SetRunning(false, false);

		}

		SetAiming(true);

	}

}

void AReclusionCharacter::OnStopAiming() {

	SetAiming(false);

}

void AReclusionCharacter::OnNextWeapon() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping)) {
			
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AReclusionWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NextWeapon);

		}

	}

}

void AReclusionCharacter::OnPrevWeapon() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping)) {

			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AReclusionWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(PrevWeapon);

		}

	}

}

void AReclusionCharacter::OnStartRunning() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (!GetCharacterMovement()->IsFalling() && GetWeapon()->GetCurrentState() != EWeaponState::Type::Firing) {

			if (IsAiming()) {

				SetAiming(false);

			}

			if (IsCrouching()) {

				SetCrouching(false);

			}

			StopWeaponFire();

			SetRunning(true, false);

		}

	}

}

void AReclusionCharacter::OnStartRunningToggle() {

	AReclusionPlayerController* MyPC = Cast<AReclusionPlayerController>(Controller);

	if (MyPC) {

		if (IsAiming()) {

			SetAiming(false);

		}

		StopWeaponFire();
		SetRunning(true, true);

	}

}

void AReclusionCharacter::OnStopRunning() {

	SetRunning(false, false);

}

void AReclusionCharacter::StartWeaponFire() {

	if (CurrentWeapon) {

		CurrentWeapon->StartFire();

	}

}

void AReclusionCharacter::StopWeaponFire() {

	if (CurrentWeapon) {

		CurrentWeapon->StopFire();

	}

}

int32 AReclusionCharacter::GetWeaponAnimID() const {

	if (GetWeapon()) {

		return GetWeapon()->GetWeaponAnimID();

	}

	return 0;

}

int32 AReclusionCharacter::GetWeaponOpticAnimID() const {

	if (GetWeapon()) {

		return GetWeapon()->GetWeaponOpticAnimID();

	}

	return 0;

}

void AReclusionCharacter::AddRecoilPitch(float Amount) {

	RecoilPitch += Amount;

}

void AReclusionCharacter::AddRecoilYaw(float Amount) {

	RecoilYaw += Amount;

}

bool AReclusionCharacter::CanFire() const {

	return true;

}

bool AReclusionCharacter::CanReload() const {

	return true;

}

void AReclusionCharacter::SetAiming(bool bNewAiming) {

	if (bIsAiming != bNewAiming) {

		PlaySoundOnActor(bNewAiming ? WeaponAimStartSound : WeaponAimStopSound);

	}

	if (bNewAiming) {

		if (GetWeapon()->GetAttachmentOptic()) {

			SetBaseAimRate(GetWeapon()->GetAttachmentOptic()->GetModifierConfig().AimingFieldOfViewMultiplier * 50.0f);

		}

	}
	else {

		SetBaseAimRate(100.0f);

	}

	bIsAiming = bNewAiming;

}

void AReclusionCharacter::SetRunning(bool bNewRunning, bool bToggle) {

	if (IsRunning() != bNewRunning) {

		if (bNewRunning) {

			PlaySoundOnActor(RunStartSound);

			if (!RunLoopAudioComponent) {

				RunLoopAudioComponent = UGameplayStatics::PlaySoundAttached(RunLoopSound, GetRootComponent());

			}

		}

	}

	bWantsToRun = bNewRunning;
	bWantsToRunToggled = bNewRunning && bToggle;

}

void AReclusionCharacter::SetCrouching(bool bNewCrouching) {

	if (bIsCrouching != bNewCrouching) {

		PlaySoundOnActor(CrouchSound);

	}

	if (bNewCrouching) {

		//Crouch();

	}
	else {

		//UnCrouch();

	}

	bIsCrouching = bNewCrouching;

}

void AReclusionCharacter::Jump() {

	if (CanJump()) {

		Super::Jump();

		if (IsRunning()) {

			SetRunning(false, false);

		}

		Stamina += StaminaConfig.StaminaJumpCost;
		Stamina = FMath::Max<float>(Stamina, StaminaConfig.StaminaMin);

		PlaySoundOnActor(JumpStartSound);

	}

}

void AReclusionCharacter::MoveForward(float Value) {

	if (Value != 0.0f) {

		AddMovementInput(GetActorForwardVector(), Value);

	}

}

void AReclusionCharacter::MoveRight(float Value) {

	if (Value != 0.0f) {

		AddMovementInput(GetActorRightVector(), Value);

	}

}

void AReclusionCharacter::TurnRate(float Rate) {

	AddControllerYawInput(Rate * BaseAimRate * GetWorld()->GetDeltaSeconds());

}

void AReclusionCharacter::TurnAtRate(float Rate) {

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AReclusionCharacter::LookUpRate(float Rate) {

	AddControllerPitchInput(Rate * BaseAimRate * GetWorld()->GetDeltaSeconds());

}

void AReclusionCharacter::LookUpAtRate(float Rate) {

	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}

FName AReclusionCharacter::GetWeaponAttachPoint() const {

	return WeaponAttachPoint;

}

float AReclusionCharacter::GetAimingSpeedModifier() const {

	return AimingSpeedModifier;

}

void AReclusionCharacter::SetBaseAimRate(float Rate) {

	BaseAimRate = Rate;

}

float AReclusionCharacter::GetStamina() const {

	return Stamina;

}

FCharacterStaminaData AReclusionCharacter::GetStaminaConfig() const {

	return StaminaConfig;

}

bool AReclusionCharacter::IsAiming() const {

	return bIsAiming;

}

float AReclusionCharacter::GetCurrentRecoil() const {

	return RecoilPitch * -1000.0f;

}

float AReclusionCharacter::GetHealth() const {

	return Health;

}

float AReclusionCharacter::GetHealthMax() const {

	return HealthConfig.HealthMax;

}

float AReclusionCharacter::GetRunningSpeedModifier() const {

	return RunningSpeedModifier;

}

float AReclusionCharacter::GetCrouchingSpeedModifier() const {

	return CrouchingSpeedModifier;

}

float AReclusionCharacter::GetCrouchingRecoilModifier() const {

	return CrouchingRecoilModifier;

}

bool AReclusionCharacter::IsRunning() const {

	if (!GetCharacterMovement()) {

		return false;

	}

	return (bWantsToRun || bWantsToRunToggled) && !GetVelocity().IsZero() && (GetVelocity().SafeNormal2D() | GetActorRotation().Vector()) > -0.1;

}

bool AReclusionCharacter::IsCrouching() const {

	if (!GetCharacterMovement()) {

		return false;

	}

	return bIsCrouching;

}