// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Energy_PROTOCharacter.h"

#include "EnergyChargeDrop.h"
#include "FPS_Energy_PROTOProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetSystemLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AFPS_Energy_PROTOCharacter

AFPS_Energy_PROTOCharacter::AFPS_Energy_PROTOCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	m_farSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FarDetectionSphereComponent"));
	m_farSphere->InitSphereRadius(250.f);
	m_farSphere->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	m_farSphere->SetGenerateOverlapEvents(true);
	m_farSphere->OnComponentBeginOverlap.AddDynamic(this, &AFPS_Energy_PROTOCharacter::OnDetectedFarHoldInteraction);
	
	m_nearSphere = CreateDefaultSubobject<USphereComponent>(TEXT("NearDetectionSphereComponent"));
	m_nearSphere->InitSphereRadius(125.f);
	m_nearSphere->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	m_nearSphere->SetGenerateOverlapEvents(true);
	m_nearSphere->OnComponentBeginOverlap.AddDynamic(this, &AFPS_Energy_PROTOCharacter::OnDetectedNearHoldInteraction);
}

void AFPS_Energy_PROTOCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	//Set the attachment rules for the spheres.
	FAttachmentTransformRules rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true);;

	//Attach the spheres to the character main mesh.
	m_farSphere->AttachToComponent(GetMesh(),rules);
	m_nearSphere->AttachToComponent(GetMesh(),rules);
}

//////////////////////////////////////////////////////////////////////////// Input

void AFPS_Energy_PROTOCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFPS_Energy_PROTOCharacter::OnPrimaryAction);

	//Bind interact event.
	PlayerInputComponent->BindAction(TEXT("Interact"),IE_Pressed, this, &AFPS_Energy_PROTOCharacter::CheckIfInteracting);
	PlayerInputComponent->BindAction(TEXT("Interact"),IE_Released,this, &AFPS_Energy_PROTOCharacter::CheckForHoldInteractors);
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFPS_Energy_PROTOCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFPS_Energy_PROTOCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFPS_Energy_PROTOCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFPS_Energy_PROTOCharacter::LookUpAtRate);
}

void AFPS_Energy_PROTOCharacter::AddCharge()
{
	//Increase the number of charges by one and check for overcharge.
	m_iNumberOfCharges++;
	if(m_iNumberOfCharges < m_iMaxNumberOfCharges)
	{
		OnAddedCharge(m_iNumberOfCharges);
	}
	else
	{
		OnAddedOverCharge();
		m_bIsOvercharged = true;
	}
}
void AFPS_Energy_PROTOCharacter::DropCharge()
{
	//Reduce the number of charges by one and reset overcharge.
	m_iNumberOfCharges--;
	OnDroppedOvercharge();
	ResetOverchargeState();
}
void AFPS_Energy_PROTOCharacter::RemoveCharges()
{
	//Remove all the charges and reset overcharge.
	OnReleaseCharges(m_iNumberOfCharges);
	m_iNumberOfCharges = 0;
	if(m_bIsOvercharged)
	{
		ResetOverchargeState();
	}
}
void AFPS_Energy_PROTOCharacter::ResetOverchargeState()
{
	//Reset overcharge parameters.
	m_bIsOvercharged = false;
	m_fOverchargeDurationCounter = 0.0f;
}

void AFPS_Energy_PROTOCharacter::StartInteracting()
{
	//Start interacting with hold interactor.
	OnStartingToInteract();
	m_holdInteractor->StartInteraction();
}
void AFPS_Energy_PROTOCharacter::StopInteracting()
{

	//If the hold interactor is invalid no need to proceed.
	if(!m_holdInteractor)
		return;
	
	OnFinishedToInteract();

	//End interaction and close reference to hold interactor.
	m_bIsHoldingInteraction = false;
	m_holdInteractor->EndInteraction();
	m_holdInteractor = nullptr;
}
void AFPS_Energy_PROTOCharacter::HandleOverchargeTimer(float& deltaTime)
{
	//Increase overcharge time.
	m_fOverchargeDurationCounter+= deltaTime;
	OnOverchargeTick(m_fOverchargeDurationCounter);

	//If overcharge time is not enough to drop the charge no need to proceed.
	if(m_fOverchargeDurationCounter < m_fOverChargeDuration)
		return;

	DropCharge();
}

void AFPS_Energy_PROTOCharacter::CheckIfInteracting()
{
	//If you are already holding an interactor no need to proceed.
	if(m_holdInteractor)
		return;

	//Get the closest non completed hold interactor.
	m_holdInteractor = DetectCloseHoldInteractables();

	//If the hold interactor is not valid no need to proceed.
	if(!m_holdInteractor)
		return;

	//If you are overcharged and you are trying to get another charge don't proceed.
	if(m_bIsOvercharged && m_holdInteractor->GetInteractorType() == PYLON
		|| m_bIsOvercharged && m_holdInteractor->GetInteractorType() == DROP)
	{
		m_holdInteractor = nullptr;
		return;
	}
	//If you are trying to release charges without holding any no need to proceed.
	if((m_holdInteractor->GetInteractorType() == CHEST && m_iNumberOfCharges < 1))
	{
		m_holdInteractor = nullptr;
		return;
	}
	
	//Reset holding parameters.
	m_bIsHoldingInteraction = true;
	m_bIsHoldInteractorComplete = false;

	StartInteracting();
}


AHoldInteractor* AFPS_Energy_PROTOCharacter::DetectCloseHoldInteractables(EHoldInteractorType typeToLookFor,float radius)
{
	//Initialize variables
	TArray<AActor*> actorsFound;
	TArray<TEnumAsByte<EObjectTypeQuery>> layersToDetect;
	TArray<AActor*> actorsToIgnore;

	//Add self to ignored actors.
	actorsToIgnore.Init(this,1);

	//Get all attached actors.
	TArray<AActor*> attachedActors;
	GetAttachedActors(attachedActors,true,true);

	//Add them to the ignored list.
	for (AActor* actor:attachedActors)
		actorsToIgnore.Add(actor);

	//Set which layers to detect.
	layersToDetect.Init(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2),1);

	//Get all actors in a certain radius.
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		radius, //TODO: replace hardcoded variable
		layersToDetect,
		nullptr,
		actorsToIgnore,
		actorsFound
		);

	//Uncomment to see debug sphere (warning : will draw a lot of sphere because of energy charge detection)
	//DrawDebugSphere(GetWorld(),GetActorLocation(),radius,18,FColor::Blue,false,5,0,1.0f);

	//If you found no actors no need to proceed.
	if(actorsFound.IsEmpty())
		return nullptr;

	//Initialize check variables.
	bool foundType{false};
	AHoldInteractor* interactor{nullptr};

	//If you are not looking for a specific type then return the closes non complete interactable.
	if(typeToLookFor == NONE)
	{
		for (AActor* actor : actorsFound)
		{
			interactor = Cast<AHoldInteractor>(actor);

			//If the actor is not a valid interactor no need to proceed.
			if(!interactor)
				continue;

			//If the interactor is complete no need to proceed.
			if(!interactor->GetIsCompleted())
			{
				foundType = true;
				break;
			}
		}
	}
	//If you are looking for a specific type return the closest non complete type actor.
	else
	{
		for (AActor* actor : actorsFound)
		{
			interactor = Cast<AHoldInteractor>(actor);
			
			//If the actor is not a valid interactor no need to proceed.
			if(!interactor)
				continue;

			//If the interactor is not the type we are looking for no need to proceed.
			if(interactor->GetInteractorType() != typeToLookFor)
				continue;
			
			//If the interactor is complete no need to proceed.
			if(!interactor->GetIsCompleted())
			{
				foundType = true;
				break;
			}
		}
	}

	//If you didn't find any actor of the type asked return a nullptr.
	return ((foundType)? interactor:nullptr);
}

void AFPS_Energy_PROTOCharacter::CheckForHoldInteractors()
{
	StopInteracting();
}
void AFPS_Energy_PROTOCharacter::HandleInteractionCompleted()
{
	//Check the action needed to take based on the interactor type.
	if(m_holdInteractor->GetInteractorType() == PYLON || m_holdInteractor->GetInteractorType() == DROP)
		AddCharge();
	else if(m_holdInteractor->GetInteractorType() == CHEST)
		RemoveCharges();

	//Stop interacting with the current hold interactable.
	StopInteracting();
}

void AFPS_Energy_PROTOCharacter::CheckForEnergyDrop()
{
	//If you are at max amount of charges no need to proceed.
	if(m_iNumberOfCharges >= m_iMaxNumberOfCharges - 1)
		return;

	//If you are overcharged or already interacting with another hold interactor no need to proceed.
	if(m_bIsHoldingInteraction || m_bIsOvercharged)
		return;

	//Get the closest interactor of type energy drop.
	m_holdInteractor = DetectCloseHoldInteractables(DROP);

	//If it's not valid no need to proceed.
	if(!m_holdInteractor)
		return;

	//Complete the interaction instantly and handle completion.
	Cast<AEnergyChargeDrop>(m_holdInteractor)->InstanInteraction();
	HandleInteractionCompleted();
}

void AFPS_Energy_PROTOCharacter::Tick(float DeltaSeconds)
{
	//Check if you can pick up energy drop.
	CheckForEnergyDrop();

	//Handle overcharge if overcharged.
	if(m_bIsOvercharged)
		HandleOverchargeTimer(DeltaSeconds);

	//If not holding any interaction no need to proceed.
	if(m_bIsHoldInteractorComplete)
		return;

	//If the interaction held is not valid no need to proceed.
	if(!m_holdInteractor)
		return;

	//Wait until the interaction complete flag is raised.
	if(m_holdInteractor->GetIsCompleted())
	{
		//Complete the interaction.
		m_bIsHoldInteractorComplete = true;
		HandleInteractionCompleted();
	}
}


void AFPS_Energy_PROTOCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AFPS_Energy_PROTOCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPS_Energy_PROTOCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AFPS_Energy_PROTOCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPS_Energy_PROTOCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPS_Energy_PROTOCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AFPS_Energy_PROTOCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AFPS_Energy_PROTOCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_Energy_PROTOCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPS_Energy_PROTOCharacter::EndTouch);

		return true;
	}
	
	return false;
}
