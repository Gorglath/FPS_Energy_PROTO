// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Energy_PROTOGameMode.h"
#include "FPS_Energy_PROTOCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFPS_Energy_PROTOGameMode::AFPS_Energy_PROTOGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AFPS_Energy_PROTOGameMode::BeginPlay()
{
	//Get all the pylons in the game.
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AEnergyPylon::StaticClass(),foundActors);

	//Check if you got valid pylons and add them to the list.
	AEnergyPylon* energyPylon;
	for (AActor* actor: foundActors)
	{
		energyPylon = Cast<AEnergyPylon>(actor);

		if(!energyPylon)
			continue;

		m_aPylons.Add(energyPylon);
	}

	//Activate random pylons.
	for (int i = 0; i < m_iNumberOfActivePylonsAtATime; i++)
		ActivateRandomPylon();
}

void AFPS_Energy_PROTOGameMode::ActivateRandomPylon()
{
	//Activate a random pylon that is not already activated.
	int index;
	do
	{
		index = FMath::RandRange(0,m_aPylons.Num() - 1);
	}while (!m_aPylons[index]->GetIsCompleted());

	m_aPylons[index]->ResetInteraction();
}



