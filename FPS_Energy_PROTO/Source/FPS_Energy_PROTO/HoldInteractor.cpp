// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldInteractor.h"


// Sets default values
AHoldInteractor::AHoldInteractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	if(m_bIsCompleted)
		return;
	
	if(!m_bIsInteracted)
		return;

	m_fHoldingTimer += DeltaTime;

	if(m_fHoldingTimer < m_fTimeToHoldForActivation)
		return;

	CompletedInteraction();
}

void AHoldInteractor::CompletedInteraction()
{
	OnCompletedInteraction();
}
