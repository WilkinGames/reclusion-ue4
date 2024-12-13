// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionWeapon.h"
#include "ReclusionWeaponTracer.h"

AReclusionWeapon::AReclusionWeapon(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = true;
	Mesh1P->bSelfShadowOnly = true;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh1P;

	MeshWeaponFrontPostSight = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponFrontPostSightMesh"));
	MeshWeaponFrontPostSight->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	MeshWeaponFrontPostSight->bReceivesDecals = false;
	MeshWeaponFrontPostSight->CastShadow = true;
	MeshWeaponFrontPostSight->bSelfShadowOnly = true;
	MeshWeaponFrontPostSight->SetCollisionObjectType(ECC_WorldDynamic);
	MeshWeaponFrontPostSight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshWeaponFrontPostSight->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshWeaponFrontPostSight->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshWeaponFrontPostSight->AttachParent = Mesh1P;

	Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bChartDistanceFactor = true;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh3P->AttachParent = Mesh1P;

	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	bFireDelay = false;
	CurrentState = EWeaponState::Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	CurrentBurst = 0;
	LastFireTime = 0.0f;

}

void AReclusionWeapon::PostInitializeComponents() {

	Super::PostInitializeComponents();

	MeshWeaponFrontPostSight->SetRelativeLocation(GetFrontPostSightLocation());
	
	UpdateWeaponAttachments();

	if (WeaponConfig.InitialClips > 0) {

		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;

	}

}

void AReclusionWeapon::Destroyed() {

	Super::Destroyed();

}

void AReclusionWeapon::StartFire() {

	if (!bWantsToFire) {

		bWantsToFire = true;
		DetermineWeaponState();

	}

	FireHandler();

}

void AReclusionWeapon::StopFire() {

	if (bWantsToFire) {

		bWantsToFire = false;
		DetermineWeaponState();

	}

	if (bPlayingFireAnim) {

		//StopWeaponAnimation(FireAnim);
		//bPlayingFireAnim = false;

	}

}

void AReclusionWeapon::StartReload() {

	if (CanReload()) {

		bPendingReload = true;
		DetermineWeaponState();

		//float AnimDuration = PlayWeaponAnimation(ReloadAnim);
		float AnimDuration = 0.0f;

		if (AnimDuration <= 0.0f) {

			AnimDuration = WeaponConfig.NoAnimReloadDuration;

		}

		GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::StopReload, AnimDuration, false);
		GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);

		if (MyPawn && MyPawn->IsLocallyControlled()) {

			//PlayWeaponSound(ReloadSound);

		}

	}

}

void AReclusionWeapon::StopReload() {

	if (CurrentState == EWeaponState::Reloading) {

		bPendingReload = false;

		DetermineWeaponState();
		
		//StopWeaponAnimation(ReloadAnim);

	}

}

void AReclusionWeapon::ReloadWeapon() {

	if (HasInfiniteAmmo()) {

		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;

	}
	else {

		if (CurrentAmmo >= WeaponConfig.AmmoPerClip) {

			CurrentAmmo -= (WeaponConfig.AmmoPerClip - CurrentAmmoInClip);
			CurrentAmmoInClip = WeaponConfig.AmmoPerClip;

		}
		else if ((CurrentAmmo + CurrentAmmoInClip) > WeaponConfig.AmmoPerClip) {

			CurrentAmmo -= (WeaponConfig.AmmoPerClip - CurrentAmmoInClip);
			CurrentAmmoInClip += (WeaponConfig.AmmoPerClip - CurrentAmmoInClip);

		}
		else {

			CurrentAmmoInClip += CurrentAmmo;
			CurrentAmmo -= CurrentAmmo;

		}

	}

}

bool AReclusionWeapon::CanFire() const {

	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));

	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));

}

bool AReclusionWeapon::CanReload() const {

	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));

	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));

}

void AReclusionWeapon::GiveAmmo(int AddAmount) {

	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);

	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	if (GetCurrentAmmoInClip() <= 0 && CanReload() && MyPawn->GetWeapon() == this) {

		StartReload();

	}

}

void AReclusionWeapon::UseAmmo() {

	if (!HasInfiniteClip()) {

		CurrentAmmoInClip --;

	}

}

void AReclusionWeapon::ApplyRecoil() {

	float RecoilPitch = FMath::FRandRange(RecoilConfig.RecoilPitchMin, RecoilConfig.RecoilPitchMax);
	float RecoilYaw = FMath::FRandRange(RecoilConfig.RecoilYawMin, RecoilConfig.RecoilYawMax);

	if (MyPawn) {

		if (MyPawn->IsAiming()) {

			RecoilPitch *= RecoilConfig.AimingModifier;
			RecoilYaw *= RecoilConfig.AimingModifier;

		}

		if (MyPawn->IsCrouching()) {

			RecoilPitch *= MyPawn->GetCrouchingRecoilModifier();
			RecoilYaw *= MyPawn->GetCrouchingRecoilModifier();

		}

		MyPawn->AddRecoilPitch(-RecoilPitch);
		MyPawn->AddRecoilYaw(-RecoilYaw);

	}

}

void AReclusionWeapon::FireHandler() {	

	if (CanFire()) {

		if (CurrentAmmoInClip > 0) {
			
			if (!bFireDelay) {

				if (MyPawn && MyPawn->IsLocallyControlled()) {				

					float ActualFireRate = WeaponConfig.FireRate;

					bFireDelay = true;

					FireWeapon();
					UseAmmo();

					if (WeaponConfig.FireMode == EWeaponFireMode::BurstFire) {

						ActualFireRate = WeaponConfig.BurstFireRate;
						CurrentBurst = WeaponConfig.ShotsPerBurst - 1;

						GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::BurstFireHandler, WeaponConfig.FireRate, false);

					}

					GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::FireDelayComplete, ActualFireRate, false);

				}

				if (GetNetMode() != NM_DedicatedServer) {

					SimulateWeaponFire();

				}

			}

		}
		else if (CanReload()) {

			StartReload();

		}
		else if (MyPawn && MyPawn->IsLocallyControlled()) {

			if (GetCurrentAmmo() == 0 && !bFireDelay) {

				//PlayWeaponSound(OutOfAmmoSound);

			}

		}

		LastFireTime = GetWorld()->GetTimeSeconds();

	}

}

void AReclusionWeapon::BurstFireHandler() {

	GetWorldTimerManager().ClearTimer(this, &AReclusionWeapon::BurstFireHandler);

	FireWeapon();
	UseAmmo();	
	
	CurrentBurst --;

	if (CurrentBurst > 0) {

		GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::BurstFireHandler, WeaponConfig.FireRate, false);

	}

	if (GetNetMode() != NM_DedicatedServer) {

		SimulateWeaponFire();

	}

}

void AReclusionWeapon::FireDelayComplete() {

	bFireDelay = false;

	GetWorldTimerManager().ClearTimer(this, &AReclusionWeapon::FireDelayComplete);

	if (bWantsToFire && WeaponConfig.FireMode == EWeaponFireMode::Automatic) {

		FireHandler();

	}

	if (CurrentAmmoInClip <= 0) {

		if (CanReload()) {

			StartReload();

		}

	}

	DetermineWeaponState();

}

void AReclusionWeapon::SimulateWeaponFire() {

	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing) {

		return;

	}

	if (MuzzleFlashStaticFX) {

		USkeletalMeshComponent* UseWeaponMesh = Mesh1P;
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashStaticFX, UseWeaponMesh, MuzzleAttachPoint);

	}

	if (MuzzleFlashDynamicFX) {

		if (FMath::FRandRange(0.0f, 1.0f) < WeaponConfig.DynamicMuzzleFXChance) {

			USkeletalMeshComponent* UseWeaponMesh = Mesh1P;
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlashDynamicFX, UseWeaponMesh, MuzzleAttachPoint);

		}

	}

	if (WeaponShell) {

		FVector SpawnLocation = GetShellEjectionLocation();
		FRotator SpawnRotation = GetActorRotation();

		UWorld* const World = GetWorld();

		if (World != NULL) {

			World->SpawnActor<AReclusionWeaponShell>(WeaponShell, SpawnLocation, SpawnRotation);

		}

	}

	if (!bPlayingFireAnim) {

		//PlayWeaponAnimation(FireAnim);

		bPlayingFireAnim = true;

	}

	if (FireAudioComponent && FireAudioComponent->IsPlaying()) {

		//FireAudioComponent->Stop();

	}

	FireAudioComponent = PlayWeaponSound(FireSound);

}

void AReclusionWeapon::OnEquip() {

	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	float Duration = 0.0f;

	if (Duration <= 0.0f) {

		Duration = 0.5f;

	}

	EquipStartedTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimer(this, &AReclusionWeapon::OnEquipFinished, Duration, false);

}

void AReclusionWeapon::OnEquipFinished() {

	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();

}

void AReclusionWeapon::OnUnEquip() {

	DetachMeshFromPawn();

	bIsEquipped = false;

	StopFire();

	if (bPendingReload) {	

		bPendingReload = false;
		
		//StopWeaponAnimation(ReloadAnim);

		GetWorldTimerManager().ClearTimer(this, &AReclusionWeapon::StopReload);
		GetWorldTimerManager().ClearTimer(this, &AReclusionWeapon::ReloadWeapon);

	}

	if (bPendingEquip) {

		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(this, &AReclusionWeapon::OnEquipFinished);

	}

	DetermineWeaponState();

}

void AReclusionWeapon::OnEnterInventory(AReclusionCharacter* NewOwner) {

	SetOwningPawn(NewOwner);

}

void AReclusionWeapon::OnLeaveInventory() {

	SetOwningPawn(NULL);

	if (bIsEquipped || bPendingEquip) {

		OnUnEquip();

	}

}

void AReclusionWeapon::SetWeaponState(EWeaponState::Type NewState) {

	const EWeaponState::Type PrevState = CurrentState;

	CurrentState = NewState;

}

void AReclusionWeapon::DetermineWeaponState() {

	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped) {

		if (bPendingReload) {

			NewState = EWeaponState::Reloading;

		}
		else if (!bPendingReload && bWantsToFire) {

			NewState = EWeaponState::Firing;

		}
		else if (bFireDelay) {

			NewState = EWeaponState::Firing;

		}

	}
	else if (bPendingEquip) {

		NewState = EWeaponState::Equipping;

	}

	SetWeaponState(NewState);

}

void AReclusionWeapon::UpdateWeaponAttachments() {

	uint32 UsingScope = true;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;

	if (AttachmentConfig.WeaponOptic == WEAPON_ATTACHMENT_OPTIC_DEFAULT) {

		UsingScope = false;

		if (AttachmentTemplates.AttachmentOpticDefault != NULL) {

			WeaponAttachmentOptic = GetWorld()->SpawnActor<AReclusionWeaponAttachment>(AttachmentTemplates.AttachmentOpticDefault, SpawnInfo);

		}

	}
	else if (AttachmentConfig.WeaponOptic == WEAPON_ATTACHMENT_OPTIC_REFLEX) {

		if (AttachmentTemplates.AttachmentOpticReflex != NULL) {

			WeaponAttachmentOptic = GetWorld()->SpawnActor<AReclusionWeaponAttachment>(AttachmentTemplates.AttachmentOpticReflex, SpawnInfo);

		}

	}
	else if (AttachmentConfig.WeaponOptic == WEAPON_ATTACHMENT_OPTIC_EOTECH) {

		if (AttachmentTemplates.AttachmentOpticEOTech != NULL) {

			WeaponAttachmentOptic = GetWorld()->SpawnActor<AReclusionWeaponAttachment>(AttachmentTemplates.AttachmentOpticEOTech, SpawnInfo);

		}

	}
	else if (AttachmentConfig.WeaponOptic == WEAPON_ATTACHMENT_OPTIC_ACOG) {

		if (AttachmentTemplates.AttachmentOpticACOG != NULL) {

			WeaponAttachmentOptic = GetWorld()->SpawnActor<AReclusionWeaponAttachment>(AttachmentTemplates.AttachmentOpticACOG, SpawnInfo);

		}

	}

	if (WeaponAttachmentOptic) {

		WeaponAttachmentOptic->SetWeapon(this);

	}

	if (UsingScope && AttachmentConfig.bHideWeaponFrontPostSightWithOptics) {

		MeshWeaponFrontPostSight->SetSkeletalMesh(NULL);

	}
	else {

		MeshWeaponFrontPostSight->SetSkeletalMesh(AttachmentTemplates.MeshFrontPostSight);

	}

}

void AReclusionWeapon::AttachMeshToPawn() {

	if (MyPawn) {

		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();

		if (MyPawn->IsLocallyControlled() == true) {

			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetMesh1P();
			Mesh1P->SetHiddenInGame(false);
			Mesh1P->AttachTo(PawnMesh1p, AttachPoint);

		}

	}

}

void AReclusionWeapon::DetachMeshFromPawn() {

	Mesh1P->DetachFromParent();
	Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromParent();
	Mesh3P->SetHiddenInGame(true);

}

FVector AReclusionWeapon::GetAdjustedAim() const {

	AReclusionPlayerController* const PlayerController = Instigator ? Cast<AReclusionPlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController) {

		FVector CamLoc;
		FRotator CamRot;

		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();

	}
	else if (Instigator) {
		
		//Handle AI controller

	}

	return FinalAim;

}

FName AReclusionWeapon::GetAttachmentOpticAttachPoint() const {

	return AttachmentOpticAttachPoint;

}

FVector AReclusionWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const {

	AReclusionPlayerController* PC = MyPawn ? Cast<AReclusionPlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC) {

		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	
	}

	return OutStartTrace;

}

FVector AReclusionWeapon::GetFrontPostSightLocation() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketLocation(FrontPostSightAttachPoint);

}

FVector AReclusionWeapon::GetAttachmentOpticLocation() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketLocation(AttachmentOpticAttachPoint);

}

FVector AReclusionWeapon::GetMuzzleLocation() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketLocation(MuzzleAttachPoint);

}

FVector AReclusionWeapon::GetMuzzleDirection() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();

}

FVector AReclusionWeapon::GetShellEjectionLocation() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketLocation(ShellAttachPoint);

}

AReclusionWeaponAttachment* AReclusionWeapon::GetAttachmentOptic() const {

	return WeaponAttachmentOptic;

}

FVector AReclusionWeapon::GetShellEjectionDirection() const {

	USkeletalMeshComponent* UseMesh = Mesh1P;

	return UseMesh->GetSocketRotation(ShellAttachPoint).Vector();

}

FWeaponData AReclusionWeapon::GetWeaponConfig() const {

	return WeaponConfig;

}

FRecoilData AReclusionWeapon::GetRecoilConfig() const {

	return RecoilConfig;

}

bool AReclusionWeapon::HasInfiniteAmmo() const {

	return WeaponConfig.bInfiniteAmmo;

}

bool AReclusionWeapon::HasInfiniteClip() const {

	return WeaponConfig.bInfiniteClip;

}

EWeaponState::Type AReclusionWeapon::GetCurrentState() const {

	return CurrentState;

}

int32 AReclusionWeapon::GetCurrentAmmo() const {

	return CurrentAmmo;

}

int32 AReclusionWeapon::GetCurrentAmmoInClip() const {

	return CurrentAmmoInClip;

}

int32 AReclusionWeapon::GetAmmoPerClip() const {

	return WeaponConfig.AmmoPerClip;

}

int32 AReclusionWeapon::GetMaxAmmo() const {

	return WeaponConfig.MaxAmmo;

}

FString AReclusionWeapon::GetWeaponName() const {

	return WeaponConfig.WeaponName;

}

FString AReclusionWeapon::GetAttachmentOpticName() const {

	if (WeaponAttachmentOptic) {

		return WeaponAttachmentOptic->GetAttachmentConfig().AttachmentName;

	}

	return FString("");

}

int32 AReclusionWeapon::GetWeaponAnimID() const {

	return WeaponAnimID;

}

int32 AReclusionWeapon::GetWeaponOpticAnimID() const {

	return AttachmentConfig.WeaponOptic;

}

void AReclusionWeapon::SetOwningPawn(AReclusionCharacter* NewOwner) {

	if (MyPawn != NewOwner) {

		Instigator = NewOwner;
		MyPawn = NewOwner;

	}

}

UAudioComponent* AReclusionWeapon::PlayWeaponSound(USoundCue* Sound) {

	UAudioComponent* AC = NULL;

	if (Sound && MyPawn) {

		AC = UGameplayStatics::PlaySoundAttached(Sound, MyPawn->GetRootComponent());

	}

	return AC;

}

float AReclusionWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation) {

	float Duration = 0.0f;

	if (MyPawn) {

		UAnimMontage* UseAnim = Animation.Pawn1P;

		if (UseAnim) {

			//Duration = MyPawn->PlayAnimMontage(UseAnim);

			Duration = MyPawn->GetMesh1P()->GetAnimInstance()->Montage_Play(UseAnim, 1.f);

		}

	}

	return Duration;

}

void AReclusionWeapon::StopWeaponAnimation(const FWeaponAnim& Animation) {

	if (MyPawn) {

		UAnimMontage* UseAnim = Animation.Pawn1P;
		
		if (UseAnim) {

			//MyPawn->StopAnimMontage(UseAnim);
			
			MyPawn->GetMesh1P()->GetAnimInstance()->Montage_Stop(0.f);

		}

	}

}