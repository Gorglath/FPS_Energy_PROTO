// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteractor.h"
#include "EnergyPylon.generated.h"

/**
 * 
 */
UCLASS()
class FPS_ENERGY_PROTO_API AEnergyPylon : public AHoldInteractor
{
	GENERATED_BODY()

public:
	AEnergyPylon();

	virtual void EndInteraction() override;
	virtual void ResetInteraction() override;
	virtual void CompletedInteraction() override;
};
