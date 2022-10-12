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

// Called when the game starts or when spawned
void AHoldInteractor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHoldInteractor::EndInteraction()
{
	if(m_eHoldInteractorType == CHEST)
		m_bIsCompleted = false;
	
	if(m_bIsCompleted)
		return;

	m_fHoldingTimer = 0.0f;
	m_bIsInteracted = false;
}
void AHoldInteractor::ResetInteraction()
{
	m_bIsCompleted = false;
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

	m_bIsCompleted = true;
	OnCompletedInteraction();
}
