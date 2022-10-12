// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteractor.h"
#include "GameFramework/GameModeBase.h"
#include "FPS_Energy_PROTOGameMode.generated.h"

UCLASS(minimalapi)
class AFPS_Energy_PROTOGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int m_iNumberOfActivePylonsAtATime{2};
	//helpers
	TArray<AHoldInteractor*> m_aPylons{};
public:
	AFPS_Energy_PROTOGameMode();
	virtual void BeginPlay() override;
	void ActivateRandomPylon();
};



