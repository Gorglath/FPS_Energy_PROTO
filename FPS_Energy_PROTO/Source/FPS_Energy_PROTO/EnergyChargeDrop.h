// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteractor.h"
#include "EnergyChargeDrop.generated.h"

/**
 * 
 */
UCLASS()
class FPS_ENERGY_PROTO_API AEnergyChargeDrop : public AHoldInteractor
{
	GENERATED_BODY()
public:
	AEnergyChargeDrop();
	virtual void CompletedInteraction() override;
	/** Call to instantly finish a hold interactor*/
	virtual void InstanInteraction();
};
