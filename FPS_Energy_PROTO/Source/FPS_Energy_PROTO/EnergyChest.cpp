// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyChest.h"

AEnergyChest::AEnergyChest() : AHoldInteractor()
{
	//Set the hold interactable parameters.
	m_bIsCompleted = false;
	m_eHoldInteractorType = CHEST;

	//Create a basic static mesh and implement it in the static mesh component.
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
	//Reset the interaction.
	m_fHoldingTimer = 0.0f;
	m_bIsCompleted = false;
	m_bIsInteracted = false;
}




