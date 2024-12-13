// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionWeaponAttachment.h"
#include "ReclusionWeapon.h"

AReclusionWeaponAttachment::AReclusionWeaponAttachment(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	MeshAttachment = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponAttachmentMesh"));
	MeshAttachment->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	MeshAttachment->bChartDistanceFactor = false;
	MeshAttachment->bReceivesDecals = false;
	MeshAttachment->CastShadow = true;
	MeshAttachment->bSelfShadowOnly = true;
	MeshAttachment->SetCollisionObjectType(ECC_WorldDynamic);
	MeshAttachment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshAttachment->SetCollisionResponseToAllChannels(ECR_Ignore);

	RootComponent = MeshAttachment;

	bCanBeDamaged = false;

}

void AReclusionWeaponAttachment::PostInitializeComponents() {

	Super::PostInitializeComponents();

}

void AReclusionWeaponAttachment::AttachMeshToWeapon() {

	if (Weapon) {

		FName AttachPoint;

		if (AttachmentConfig.AttachmentType == EAttachmentType::Type::Optic) {

			AttachPoint = Weapon->GetAttachmentOpticAttachPoint();

		}

		USkeletalMeshComponent* WeaponMesh1P = Weapon->GetMesh1P();

		MeshAttachment->SetHiddenInGame(false);
		MeshAttachment->AttachTo(WeaponMesh1P, AttachPoint);

	}

}

void AReclusionWeaponAttachment::SetWeapon(class AReclusionWeapon* NewWeapon) {

	if (NewWeapon) {

		Weapon = NewWeapon;

		AttachMeshToWeapon();

	}

}

void AReclusionWeaponAttachment::SetZoomSceneCapture(class USceneCaptureComponent2D* NewZoomSceneCapture) {

	if (NewZoomSceneCapture) {

		ZoomSceneCapture = NewZoomSceneCapture;

	}

}

FAttachmentData AReclusionWeaponAttachment::GetAttachmentConfig() const {

	return AttachmentConfig;

}

FAttachmentModifierData AReclusionWeaponAttachment::GetModifierConfig() const {

	return ModifierConfig;

}

FAttachmentZoomData AReclusionWeaponAttachment::GetZoomConfig() const {

	return ZoomConfig;

}

USceneCaptureComponent2D* AReclusionWeaponAttachment::GetZoomSceneCapture() const {

	return ZoomSceneCapture;

}

bool AReclusionWeaponAttachment::HasZoomEnabled() const {

	return ZoomConfig.bZoomEnabled;

}