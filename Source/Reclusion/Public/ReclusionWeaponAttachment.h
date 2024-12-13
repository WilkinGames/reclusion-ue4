// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ReclusionWeaponAttachment.generated.h"

UENUM()
namespace EAttachmentType {

	enum Type {
		Optic,
		Rail,
		Underbarrel,
	};

}

USTRUCT()
struct FAttachmentData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	FString AttachmentID;

	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	FString AttachmentName;

	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	TEnumAsByte<EAttachmentType::Type> AttachmentType;

};

USTRUCT()
struct FAttachmentModifierData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Optics)
	float AimingFieldOfViewMultiplier;

	FAttachmentModifierData() {

		AimingFieldOfViewMultiplier = 1.0f;

	}

};

USTRUCT()
struct FAttachmentZoomData {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	bool bZoomEnabled;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float ZoomAmount;

	FAttachmentZoomData() {

		bZoomEnabled = false;
		ZoomAmount = 0.0f;

	}

};

/**
 * Weapon attachment class.
 */
UCLASS(Blueprintable)
class RECLUSION_API AReclusionWeaponAttachment : public AActor {

	GENERATED_UCLASS_BODY()

private:

	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FAttachmentData AttachmentConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FAttachmentModifierData ModifierConfig;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FAttachmentZoomData ZoomConfig;

	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	class USkeletalMeshComponent* MeshAttachment;

	UPROPERTY(VisibleDefaultsOnly, Category=Weapon)
	class AReclusionWeapon* Weapon;

	UPROPERTY(VisibleDefaultsOnly, Category=Zoom)
	class USceneCaptureComponent2D* ZoomSceneCapture;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	FName ZoomAttachPoint;

	void AttachMeshToWeapon();

public:

	void SetWeapon(class AReclusionWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetZoomSceneCapture(class USceneCaptureComponent2D* NewZoomSceneCapture);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	FAttachmentData GetAttachmentConfig() const;

	UFUNCTION(BlueprintCallable, Category=Attachment)
	FAttachmentModifierData GetModifierConfig() const;

	UFUNCTION(BlueprintCallable, Category=Attachment)
	FAttachmentZoomData GetZoomConfig() const;

	UFUNCTION(BlueprintCallable, Category=Attachment)
	USceneCaptureComponent2D* GetZoomSceneCapture() const;

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool HasZoomEnabled() const;
	
};