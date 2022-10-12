// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteractor.h"
#include "GameFramework/Character.h"
#include "FPS_Energy_PROTOCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class AFPS_Energy_PROTOCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	
	//helpers
	AHoldInteractor* m_holdInteractor{nullptr};
	int m_iNumberOfCharges{0};
	float m_fOverchargeDurationCounter{0.0f};
	bool m_bIsHoldInteractorComplete{true};
	bool m_bIsOvercharged{false};
public:
	AFPS_Energy_PROTOCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Electic Charges")
	int m_iMaxNumberOfCharges{3};
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Electic Charges")
	float m_fOverChargeDuration{10.0f};
	//Blueprint callable events.
	UFUNCTION(BlueprintImplementableEvent)
	void OnAddedCharge(int currentNumberOfCharges);
	UFUNCTION(BlueprintImplementableEvent)
	void OnReleaseCharges();
	UFUNCTION(BlueprintImplementableEvent)
	void OnAddedOverCharge();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDroppedOvercharge();
protected:
	
	/** Fires a projectile. */
	void OnPrimaryAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	/** Adds a single electric charge to the player */
	virtual void AddCharge();
	/** Drops a single electric charge from the player */
	virtual void DropCharge();
	/** Remove all electric charge from the player */
	virtual void RemoveCharges();
	/** Called via input to run an overlap sphere check, detects hold interactors  */
	virtual void CheckForHoldInteractors();
	/** Called via input to handle the release of hold interactors*/
	virtual void CheckIfInteracting();
	/** Handle the starting of interaction with hold interactors*/
	virtual void StartInteracting();
	/** Handle the ending of interaction with hold interactors*/
	virtual void StopInteracting();
	/** Handle the completion of interaction with hold interactors*/
	virtual void HandleInteractionCompleted();
	/** Handle the updating and handling of overcharge*/
	virtual void HandleOverchargeTimer(float& deltaTime);
	/** Resets the overcharge parameters*/
	virtual void ResetOverchargeState();
	/** Check for energy charge drop collision*/
	virtual void CheckForEnergyDrop();
	/** Overlap sphere to detect hold interactables, return the first one hit*/
	virtual AHoldInteractor* DetectCloseHoldInteractables(EHoldInteractorType typeToLookFor = NONE);
	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

