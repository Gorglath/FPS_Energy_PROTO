// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteractor.h"
#include "EnergyChest.generated.h"

/**
 * 
 */
UCLASS()
class FPS_ENERGY_PROTO_API AEnergyChest : public AHoldInteractor
{
	GENERATED_BODY()
public:
	AEnergyChest();

	virtual void EndInteraction() override;
	virtual void CompletedInteraction() override;
	
};
