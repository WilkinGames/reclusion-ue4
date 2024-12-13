// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ReclusionCharacter.h"
#include "ReclusionPlayerController.h"
#include "ReclusionWeaponTracer.h"
#include "ReclusionWeaponShell.h"
#include "ReclusionWeaponAttachment.h"
#include "ReclusionWeapon.generated.h"

UENUM()
namespace EWeaponType {

	enum Type {
		Handgun,
		SMG,
		Shotgun,
		Rifle,
		Sniper,
	};

}

UENUM()
namespace EWeaponState {

	enum Type {
		Idle,
		Firing,
		Reloading,
		Equipping,
	};

}

UENUM()
namespace EWeaponFireMode {

	enum Type {
		Automatic,
		BurstFire,
		SemiAutomatic,
	};

}

UENUM()
namespace EWeaponAttachmentOptic {

	enum Type {
		Default,
		Reflex,
		EOTech,
		ACOG,
	};

}

USTRUCT()
struct FWeaponData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=WeaponInfo)
	FString WeaponID;

	UPROPERTY(EditDefaultsOnly, Category=WeaponInfo)
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category=WeaponInfo)
	TEnumAsByte<EWeaponType::Type> WeaponType;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteAmmo;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteClip;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 AmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 InitialClips;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 ShotsPerBurst;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float BurstFireRate;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	TEnumAsByte<EWeaponFireMode::Type> FireMode;

	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float NoAnimReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	float DynamicMuzzleFXChance;

	FWeaponData() {

		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 300;
		AmmoPerClip = 30;
		InitialClips = 3;
		FireRate = 0.1f;
		BurstFireRate = 0.5;
		ShotsPerBurst = 3;
		FireMode = EWeaponFireMode::Automatic;
		NoAnimReloadDuration = 1.0f;
		DynamicMuzzleFXChance = 0.2f;

	}

};

USTRUCT()
struct FRecoilData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float RecoilPitchMin;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float RecoilPitchMax;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float RecoilYawMin;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float RecoilYawMax;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float InterpKickSpeed;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float InterpRecoverySpeed;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	float AimingModifier;

	UPROPERTY(EditDefaultsOnly, Category=Recoil)
	bool AddYawAsRoll;

	FRecoilData() {

		RecoilPitchMin = 0.5f;
		RecoilPitchMax = 0.5f;
		RecoilYawMin = -0.2f;
		RecoilYawMax = 0.2f;
		InterpKickSpeed = 0.01f;
		InterpRecoverySpeed = 0.01f;
		AimingModifier = 0.2f;
		AddYawAsRoll = true;

	}

};

USTRUCT()
struct FWeaponAttachments {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Sights)
	USkeletalMesh* MeshFrontPostSight;

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	TSubclassOf<class AReclusionWeaponAttachment> AttachmentOpticDefault;

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	TSubclassOf<class AReclusionWeaponAttachment> AttachmentOpticReflex;

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	TSubclassOf<class AReclusionWeaponAttachment> AttachmentOpticEOTech;

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	TSubclassOf<class AReclusionWeaponAttachment> AttachmentOpticACOG;

};

USTRUCT()
struct FWeaponAttachmentData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Sights)
	uint32 bHideWeaponFrontPostSightWithOptics : 1;

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	TEnumAsByte<EWeaponAttachmentOptic::Type> WeaponOptic;

	FWeaponAttachmentData() {

		bHideWeaponFrontPostSightWithOptics = true;
		WeaponOptic = EWeaponAttachmentOptic::Type::Default;

	}

};

USTRUCT()
struct FWeaponAnim {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* Pawn1P;

	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* Pawn3P;

};

/**
 * Abstract weapon class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionWeapon : public AActor {

	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

protected:

	UPROPERTY(Transient)
	class AReclusionCharacter* MyPawn;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FRecoilData RecoilConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponAttachmentData AttachmentConfig;

	UPROPERTY(EditDefaultsOnly, Category=Attachments)
	FWeaponAttachments AttachmentTemplates;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* FireSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Sound)
	UAudioComponent* FireAudioComponent;

	UPROPERTY(EditDefaultsOnly, Category=Animation)
	int32 WeaponAnimID;

	UPROPERTY(VisibleDefaultsOnly, Category=Attachments)
	class USkeletalMeshComponent* MeshWeaponFrontPostSight;

	UPROPERTY(VisibleDefaultsOnly, Category=Attachments)
	class AReclusionWeaponAttachment* WeaponAttachmentOptic;

	UPROPERTY(EditDefaultsOnly, Category=Attachments)
	FName FrontPostSightAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category=Attachments)
	FName AttachmentOpticAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FName ShellAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* MuzzleFlashStaticFX;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* MuzzleFlashDynamicFX;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class AReclusionWeaponTracer> WeaponTracer;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class AReclusionWeaponShell> WeaponShell;

	uint32 bPlayingFireAnim : 1;

	uint32 bIsEquipped : 1;

	uint32 bWantsToFire : 1;

	uint32 bPendingReload : 1;

	uint32 bPendingEquip : 1;

	uint32 bFireDelay;

	EWeaponState::Type CurrentState;

	float LastFireTime;

	float EquipStartedTime;

	float EquipDuration;

	UPROPERTY(Transient)
	int32 CurrentAmmo;

	UPROPERTY(Transient)
	int32 CurrentAmmoInClip;

	UPROPERTY(Transient)
	int32 CurrentBurst;

	virtual void FireWeapon() PURE_VIRTUAL(AReclusionWeapon::FireWeapon, );

	virtual void SimulateWeaponFire();

	void FireHandler();

	void BurstFireHandler();

	void FireDelayComplete();

	void SetWeaponState(EWeaponState::Type NewState);

	void DetermineWeaponState();

	void UpdateWeaponAttachments();

	void AttachMeshToPawn();

	void DetachMeshFromPawn();

	void GiveAmmo(int AddAmount);

	void UseAmmo();

	void ApplyRecoil();

	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	float PlayWeaponAnimation(const FWeaponAnim& Animation);

	void StopWeaponAnimation(const FWeaponAnim& Animation);

public:

	virtual void StartFire();

	virtual void StopFire();

	virtual void StartReload();

	virtual void StopReload();

	virtual void ReloadWeapon();

	bool CanFire() const;

	bool CanReload() const;

	EWeaponState::Type GetCurrentState() const;

	UFUNCTION(BlueprintCallable, Category=Weapon)
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category=Weapon)
	int32 GetCurrentAmmoInClip() const;

	int32 GetAmmoPerClip() const;

	int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category=Weapon)
	FString GetWeaponName() const;

	UFUNCTION(BlueprintCallable, Category=Weapon)
	FString GetAttachmentOpticName() const;

	int32 GetWeaponAnimID() const;

	int32 GetWeaponOpticAnimID() const;

	virtual void OnEquip();

	virtual void OnEquipFinished();

	virtual void OnUnEquip();

	virtual void OnEnterInventory(AReclusionCharacter* NewOwner);

	virtual void OnLeaveInventory();

	virtual FVector GetAdjustedAim() const;

	FName GetAttachmentOpticAttachPoint() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FVector GetFrontPostSightLocation() const;

	FVector GetAttachmentOpticLocation() const;

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	FVector GetShellEjectionLocation() const;

	FVector GetShellEjectionDirection() const;

	AReclusionWeaponAttachment* GetAttachmentOptic() const;

	FWeaponData GetWeaponConfig() const;

	FRecoilData GetRecoilConfig() const;

	bool HasInfiniteAmmo() const;

	bool HasInfiniteClip() const;

	void SetOwningPawn(AReclusionCharacter* NewOwner);

	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	
};