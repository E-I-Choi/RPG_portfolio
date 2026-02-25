// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPG_portfolioGameMode.h"
#include "RPG_portfolioPlayerController.h"
#include "RPG_portfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPG_portfolioGameMode::ARPG_portfolioGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARPG_portfolioPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}