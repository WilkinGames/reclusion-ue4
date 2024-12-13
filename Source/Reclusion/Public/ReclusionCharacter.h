// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "ReclusionCharacter.generated.h"

USTRUCT()
struct FCharacterHealthData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Health)
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, Category=Health)
	float HealthBaseRecoveryRate;

	FCharacterHealthData() {

		HealthMax = 100.0f;
		HealthBaseRecoveryRate = 0.05f;

	}

};

USTRUCT()
struct FCharacterStaminaData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaMax;

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaMin;

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaRunningDrainRate;

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaBaseRecoveryRate;

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaCrouchingRecoveryMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=Stamina)
	float StaminaJumpCost;

	FCharacterStaminaData() {

		StaminaMax = 100.0f;
		StaminaMin = 0.0f;
		StaminaRunningDrainRate = -0.1f;
		StaminaBaseRecoveryRate = 0.05f;
		StaminaCrouchingRecoveryMultiplier = 1.2f;
		StaminaJumpCost = -10.0f;

	}

};

USTRUCT()
struct FCharacterCameraShakeData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UReclusionCameraShake> Idle;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UReclusionCameraShake> IdleLowStamina;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UReclusionCameraShake> MovementWalking;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UReclusionCameraShake> MovementCrouching;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UReclusionCameraShake> MovementRunning;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	TSubclassOf<class UCameraShake> JumpLand;

};

USTRUCT()
struct FCharacterFootstepData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Sounds)
	USoundCue* Generic;

};

/**
 * Primary character class.
 */
UCLASS(config=Game)
class AReclusionCharacter : public ACharacter {

	GENERATED_BODY()

private:

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FirstPersonCameraComponent;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FCharacterHealthData HealthConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FCharacterStaminaData StaminaConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FCharacterCameraShakeData CameraShakeConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FCharacterFootstepData FootstepConfig;

	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	FName WeaponAttachPoint;

	UPROPERTY(VisibleDefaultsOnly, Category=Camera)
	TSubclassOf<class UReclusionCameraShake> MovementCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	TArray<TSubclassOf<class AReclusionWeapon>> DefaultInventoryClasses;

	UPROPERTY(Transient)
	TArray<class AReclusionWeapon*> Inventory;

	UPROPERTY(Transient)
	class AReclusionWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* CrouchSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* RunStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* RunStopSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* RunLoopSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* JumpStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* JumpLandSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* WeaponAimStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Sound)
	USoundCue* WeaponAimStopSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Sound)
	UAudioComponent* RunLoopAudioComponent;

	float DepthOfFieldDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=UI)
	float CrosshairSpread;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float RecoilPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float RecoilPitchRecovery;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float RecoilYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float RecoilYawRecovery;

	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	float AimingSpeedModifier;

	UPROPERTY(VisibleDefaultsOnly, Category=Pawn)
	float Health;

	UPROPERTY(VisibleDefaultsOnly, Category=Pawn)
	float Stamina;	

	UPROPERTY(VisibleDefaultsOnly, Category=Pawn)
	float BaseStepSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Pawn)
	float StepCounter;

	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	float RunningSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	float CrouchingSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	float CrouchingRecoilModifier;

	UPROPERTY(Transient)
	float DecBEH;

	UPROPERTY(Transient)
	float DecCapsHeight;

	UPROPERTY(Transient)
	uint8 bIsCrouching : 1;

	UPROPERTY(Transient)
	uint8 bIsAiming : 1;

	UPROPERTY(Transient)
	uint8 bWantsToRun : 1;

	UPROPERTY(Transient)
	uint8 bWantsToRunToggled : 1;

	UPROPERTY(Transient)
	uint8 bWantsToFire : 1;

	void SpawnDefaultInventory();

	void DestroyInventory();

	void SetCurrentWeapon(class AReclusionWeapon* NewWeapon, class AReclusionWeapon* LastWeapon = NULL);

	void AddWeapon(class AReclusionWeapon* Weapon);

	void RemoveWeapon(class AReclusionWeapon* Weapon);

	void EquipWeapon(class AReclusionWeapon* Weapon);

	void OnStartCrouching();

	void OnStopCrouching();

	void OnStartFire();

	void OnStopFire();

	void OnReload();

	void OnStartAiming();

	void OnStopAiming();

	void OnNextWeapon();

	void OnPrevWeapon();

	void OnStartRunning();

	void OnStartRunningToggle();

	void OnStopRunning();

	void CrouchTick(float DeltaTime);

	void SetAiming(bool bNewAiming);

	void SetRunning(bool bNewRunning, bool bToggle);

	void SetCrouching(bool bNewCrouching);

	void StartWeaponFire();

	void StopWeaponFire();

	void Jump() override;

	void MoveForward(float Val);

	void MoveRight(float Val);

	void TurnRate(float Rate);

	void TurnAtRate(float Rate);

	void LookUpRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera)
	float BaseAimRate;

	AReclusionCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void OnLanded(const FHitResult& Hit) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AReclusionWeapon* GetWeapon() const;

	FName GetWeaponAttachPoint() const;

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	FCharacterStaminaData GetStaminaConfig() const;

	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	int32 GetWeaponAnimID() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	int32 GetWeaponOpticAnimID() const;

	void AddRecoilPitch(float Amount);

	void AddRecoilYaw(float Amount);

	bool CanFire() const;

	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category="Game|Camera")
	void SetBaseAimRate(float Rate);

	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	float GetAimingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	bool IsAiming() const;

	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	float GetCurrentRecoil() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetRunningSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetCrouchingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	float GetCrouchingRecoilModifier() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category=Pawn)
	bool IsCrouching() const;

	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};