// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyPylon.h"

#include "FPS_Energy_PROTOGameMode.h"

AEnergyPylon::AEnergyPylon(): AHoldInteractor()
{
	m_bIsCompleted = true;
	m_eHoldInteractorType = PYLON;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> interactorMesh(TEXT("/Game/LevelPrototyping/Meshes/SM_Cylinder.SM_Cylinder"));
	if(interactorMesh.Succeeded())
	{
		m_smInteractorMeshComponent->SetStaticMesh(interactorMesh.Object);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2,ECR_Block);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	}
}

void AEnergyPylon::CompletedInteraction()
{
	Super::CompletedInteraction();

	Cast<AFPS_Energy_PROTOGameMode>(GetWorld()->GetAuthGameMode())->ActivateRandomPylon();

	m_bIsCompleted = true;
}

void AEnergyPylon::ResetInteraction()
{
	Super::ResetInteraction();
	m_bIsCompleted = false;
	m_fHoldingTimer = 0.0f;
	m_bIsInteracted = false;
}

void AEnergyPylon::EndInteraction()
{
	if(m_bIsCompleted)
		return;

	m_fHoldingTimer = 0.0f;
	m_bIsInteracted = false;
}


