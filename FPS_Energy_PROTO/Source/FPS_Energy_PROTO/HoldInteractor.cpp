// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldInteractor.h"


// Sets default values
AHoldInteractor::AHoldInteractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a static mesh component and declare it as root.
	m_smInteractorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactor mesh"));
	RootComponent = m_smInteractorMeshComponent;
	
}

void AHoldInteractor::EndInteraction()
{
	
}
void AHoldInteractor::ResetInteraction()
{
	OnResetInteraction();
}
void AHoldInteractor::StartInteraction()
{
	OnStartedInteraction();
	m_bIsInteracted = true;
}

// Called every frame
void AHoldInteractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If you're complete no need to proceed.
	if(m_bIsCompleted)
		return;

	//If the player stopped interacting without completing the hold then stop counting.
	if(!m_bIsInteracted)
		return;

	m_fHoldingTimer += DeltaTime;

	//If not enough time has passed since the start of the hold don't proceed.
	if(m_fHoldingTimer < m_fTimeToHoldForActivation)
		return;

	//Complete the interaction.
	CompletedInteraction();
}

void AHoldInteractor::CompletedInteraction()
{
	OnCompletedInteraction();
}
