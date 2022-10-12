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
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AHoldInteractor::StaticClass(),foundActors);

	AHoldInteractor* holdInteractor;
	for (AActor* actor: foundActors)
	{
		holdInteractor = Cast<AHoldInteractor>(actor);

		if(!holdInteractor)
			continue;

		if(holdInteractor->GetInteractorType() == PYLON)
			m_aPylons.Add(holdInteractor);
	}

	for (int i = 0; i < m_iNumberOfActivePylonsAtATime; i++)
		ActivateRandomPylon();
}

void AFPS_Energy_PROTOGameMode::ActivateRandomPylon()
{
	int index;
	do
	{
		index = FMath::RandRange(0,m_aPylons.Num() - 1);
	}while (!m_aPylons[index]->GetIsCompleted());

	m_aPylons[index]->ResetInteraction();
}



