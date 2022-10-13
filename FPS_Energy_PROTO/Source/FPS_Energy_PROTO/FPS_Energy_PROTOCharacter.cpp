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
	FAttachmentTransformRules rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true);;
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
	m_iNumberOfCharges--;
	OnDroppedOvercharge();
	ResetOverchargeState();
}
void AFPS_Energy_PROTOCharacter::RemoveCharges()
{
	OnReleaseCharges(m_iNumberOfCharges);
	m_iNumberOfCharges = 0;
	if(m_bIsOvercharged)
	{
		ResetOverchargeState();
	}
}
void AFPS_Energy_PROTOCharacter::ResetOverchargeState()
{
	m_bIsOvercharged = false;
	m_fOverchargeDurationCounter = 0.0f;
}

void AFPS_Energy_PROTOCharacter::StartInteracting()
{
	OnStartingToInteract();
	m_holdInteractor->StartInteraction();
}
void AFPS_Energy_PROTOCharacter::StopInteracting()
{
	if(!m_holdInteractor)
		return;
	
	OnFinishedToInteract();
	m_bIsHoldingInteraction = false;
	m_holdInteractor->EndInteraction();
	m_holdInteractor = nullptr;
}
void AFPS_Energy_PROTOCharacter::HandleOverchargeTimer(float& deltaTime)
{
	m_fOverchargeDurationCounter+= deltaTime;
	OnOverchargeTick(m_fOverchargeDurationCounter);
	if(m_fOverchargeDurationCounter < m_fOverChargeDuration)
		return;

	DropCharge();
}

void AFPS_Energy_PROTOCharacter::CheckIfInteracting()
{
	if(m_holdInteractor)
		return;
	
	m_holdInteractor = DetectCloseHoldInteractables();
	
	if(!m_holdInteractor)
		return;
	
	if(m_bIsOvercharged && m_holdInteractor->GetInteractorType() == PYLON)
		return;
	
	m_bIsHoldingInteraction = true;
	if(m_holdInteractor->GetIsCompleted()
		||(m_holdInteractor->GetInteractorType() == CHEST && m_iNumberOfCharges < 1))
	{
		m_holdInteractor = nullptr;
		return;
	}

	m_bIsHoldInteractorComplete = false;

	StartInteracting();
}


AHoldInteractor* AFPS_Energy_PROTOCharacter::DetectCloseHoldInteractables(EHoldInteractorType typeToLookFor,float radius)
{	TArray<AActor*> actorsFound;
	TArray<TEnumAsByte<EObjectTypeQuery>> layersToDetect;
	TArray<AActor*> actorsToIgnore;

	actorsToIgnore.Init(this,1);

	TArray<AActor*> attachedActors;
	GetAttachedActors(attachedActors,true,true);

	for (AActor* actor:attachedActors)
		actorsToIgnore.Add(actor);
	
	layersToDetect.Init(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2),1);
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		radius, //TODO: replace hardcoded variable
		layersToDetect,
		nullptr,
		actorsToIgnore,
		actorsFound
		);

	//DrawDebugSphere(GetWorld(),GetActorLocation(),radius,18,FColor::Blue,false,5,0,1.0f);
	
	if(actorsFound.IsEmpty())
		return nullptr;
	
	bool foundType{false};
	AHoldInteractor* interactor{nullptr};
	if(typeToLookFor == NONE)
	{
		for (AActor* actor : actorsFound)
		{
			interactor = Cast<AHoldInteractor>(actor);
			if(!interactor)
				continue;

			if(!interactor->GetIsCompleted())
			{
				foundType = true;
				break;
			}
		}
	}
	else
	{
		for (AActor* actor : actorsFound)
		{
			interactor = Cast<AHoldInteractor>(actor);
			if(!interactor)
				continue;

			if(interactor->GetInteractorType() == typeToLookFor)
			{
				foundType = true;
				break;
			}
		}
	}
	return ((foundType)? interactor:nullptr);
}

void AFPS_Energy_PROTOCharacter::CheckForHoldInteractors()
{
	StopInteracting();
}
void AFPS_Energy_PROTOCharacter::HandleInteractionCompleted()
{
	switch (m_holdInteractor->GetInteractorType())
	{
	case NONE:
		break;
	case PYLON:
		AddCharge();
		break;
	case CHEST:
		RemoveCharges();
		break;
	case DROP:
		AddCharge();
	}
	StopInteracting();
}

void AFPS_Energy_PROTOCharacter::CheckForEnergyDrop()
{
	if(m_iNumberOfCharges >= m_iMaxNumberOfCharges - 1)
		return;

	if(m_bIsHoldingInteraction || m_bIsOvercharged)
		return;
	m_holdInteractor = DetectCloseHoldInteractables(DROP);
	
	if(!m_holdInteractor)
		return;

	Cast<AEnergyChargeDrop>(m_holdInteractor)->InstanInteraction();
	HandleInteractionCompleted();
}

void AFPS_Energy_PROTOCharacter::Tick(float DeltaSeconds)
{
	CheckForEnergyDrop();
	
	UE_LOG(LogTemp,Warning,TEXT("CHECKING 1"));
	if(m_bIsOvercharged)
		HandleOverchargeTimer(DeltaSeconds);
	
	UE_LOG(LogTemp,Warning,TEXT("CHECKING 2"));
	if(m_bIsHoldInteractorComplete)
		return;

	UE_LOG(LogTemp,Warning,TEXT("CHECKING 3"));
	if(!m_holdInteractor)
		return;

	UE_LOG(LogTemp,Warning,TEXT("CHECKING"));
	if(m_holdInteractor->GetIsCompleted())
	{
	UE_LOG(LogTemp,Warning,TEXT("COMPLETED"));
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
