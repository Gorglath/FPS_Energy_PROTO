// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyChest.h"

AEnergyChest::AEnergyChest() : AHoldInteractor()
{
	m_bIsCompleted = false;
	m_eHoldInteractorType = CHEST;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> interactorMesh(TEXT("/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	if(interactorMesh.Succeeded())
	{
		m_smInteractorMeshComponent->SetStaticMesh(interactorMesh.Object);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2,ECR_Block);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	}
}

void AEnergyChest::CompletedInteraction()
{
	m_bIsCompleted = true;
	Super::CompletedInteraction();
}

void AEnergyChest::EndInteraction()
{
	m_fHoldingTimer = 0.0f;
	m_bIsCompleted = false;
	m_bIsInteracted = false;
}




