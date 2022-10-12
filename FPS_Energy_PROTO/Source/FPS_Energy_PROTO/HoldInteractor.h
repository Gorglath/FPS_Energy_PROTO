// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoldInteractor.generated.h"
UENUM()
enum EHoldInteractorType
{
	NONE UMETA("NONE"),
	PYLON UMETA("Energy Pylon"),
	CHEST UMETA("Energy Chest"),
	DROP UMETA("Energy charge drop")
	
};
UCLASS()
class FPS_ENERGY_PROTO_API AHoldInteractor : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Hold Interactor")
	float m_fTimeToHoldForActivation{1.0f};
	
	UPROPERTY(EditAnywhere, Category = "Hold Interactor")
	TEnumAsByte<EHoldInteractorType> m_eHoldInteractorType{NONE};
	//helpers
	bool m_bIsInteracted{false};
	bool m_bIsCompleted{true};
	float m_fHoldingTimer{0.0f};
public:	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* m_smInteractorMeshComponent{nullptr};
	// Sets default values for this actor's properties
	AHoldInteractor();

    //Blueprint callable events.
	UFUNCTION(BlueprintImplementableEvent, Category = "Hold Interactor")
	void OnCompletedInteraction();
	UFUNCTION(BlueprintImplementableEvent, Category = "Hold Interactor")
	void OnStartedInteraction();
	UFUNCTION(BlueprintImplementableEvent, Category = "Hold Interactor")
	void OnResetInteraction();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartInteraction();

	virtual void EndInteraction();

	virtual void ResetInteraction();

	virtual void CompletedInteraction();
	
	bool GetIsCompleted() const{return m_bIsCompleted;}
	EHoldInteractorType GetInteractorType() const{return m_eHoldInteractorType;}
};
