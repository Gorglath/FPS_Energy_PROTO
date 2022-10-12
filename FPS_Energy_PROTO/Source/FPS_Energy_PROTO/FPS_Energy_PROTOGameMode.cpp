// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Energy_PROTOGameMode.h"
#include "FPS_Energy_PROTOCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_Energy_PROTOGameMode::AFPS_Energy_PROTOGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
