// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyChargeDrop.h"

AEnergyChargeDrop::AEnergyChargeDrop() : AHoldInteractor()
{
	m_bIsCompleted = false;
	m_eHoldInteractorType = DROP;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> interactorMesh(TEXT("/Game/FPWeapon/Mesh/FirstPersonProjectileMesh.FirstPersonProjectileMesh"));
	if(interactorMesh.Succeeded())
	{
		m_smInteractorMeshComponent->SetStaticMesh(interactorMesh.Object);
		m_smInteractorMeshComponent->SetWorldScale3D(FVector(0.1f));
		m_smInteractorMeshComponent->SetSimulatePhysics(true);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
		m_smInteractorMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2,ECR_Block);
	}
}
void AEnergyChargeDrop::CompletedInteraction()
{
	m_bIsCompleted = true;
	OnCompletedInteraction();
	Destroy();
}

void AEnergyChargeDrop::InstanInteraction()
{
	CompletedInteraction();
}



