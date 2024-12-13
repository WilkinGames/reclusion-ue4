// Copyright 2014-2015 Nailfex, Inc. All Rights Reserved.

#include "Reclusion.h"
#include "ReclusionGameMode.h"
#include "ReclusionHUD.h"
#include "ReclusionCharacter.h"
#include "ReclusionPlayerController.h"

AReclusionGameMode::AReclusionGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Characters/PlayerCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/Controllers/PlayerController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;

	HUDClass = AReclusionHUD::StaticClass();

	//PlayerControllerClass = AReclusionPlayerController::StaticClass();

}