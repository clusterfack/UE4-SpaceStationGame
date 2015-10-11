// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SpaceStationGame.h"
#include "SpaceStationGameCharacter.h"
#include "SpaceStationGamePlayerController.h"
#include "SpaceStationGameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Limb.h"
#include "Inventory.h"
#include "GameFramework/InputSettings.h"

#include "Item.h"
#include "InstancedItemContainer.h"
#include "ChemDispenser.h"
#include "Reagents.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASpaceStationGameCharacter

ASpaceStationGameCharacter::ASpaceStationGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	LeftArm = CreateDefaultSubobject<ULimb>(TEXT("Left Arm"));
	RightArm = CreateDefaultSubobject<ULimb>(TEXT("Right Arm"));

	Health = 100.f;

	MaxUseDistance = 200.f;
	MaxDropDistance = 200.f;

	bReplicates = true;
	bReplicateMovement = true;
}

void ASpaceStationGameCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to only owner
	DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, Health, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, BloodType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, BloodVolume, COND_OwnerOnly);

	//DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, InventoryStruct, COND_OwnerOnly);
	DOREPLIFETIME(ASpaceStationGameCharacter, InventoryStruct);
	DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, SelectedItem, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpaceStationGameCharacter, AffectedItem, COND_OwnerOnly);

	DOREPLIFETIME(ASpaceStationGameCharacter, PawnName);
}

void ASpaceStationGameCharacter::BeginPlay()
{

}

void ASpaceStationGameCharacter::Tick(float DeltaSeconds)
{
	if (!HasAuthority() && InventoryItemIsValid(GetSelectedItem()))
	{
		AItem* Item = GetInventoryItem(GetSelectedItem());
		if (Item->bBeingDropped)
		{
			//Line trace for usable objects.
			FVector CamLoc;
			FRotator CamRot;

			GetController()->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
			const FVector StartTrace = CamLoc; // trace start is the camera location
			const FVector Direction = CamRot.Vector();
			const FVector EndTrace = StartTrace + Direction * MaxDropDistance; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

			// Perform trace to retrieve hit info
			FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
			TraceParams.bTraceAsyncScene = true;
			TraceParams.bReturnPhysicalMaterial = true;

			FHitResult Hit(ForceInit);

			if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams))
			{
				if (Item->bUseGrid && Item->GridSize.X > 0 && Item->GridSize.Y > 0 && Item->GridSize.Z > 0)
				{
					Hit.Location /= Item->GridSize;

					int32 X = Hit.Location.X;
					int32 Y = Hit.Location.Y;
					int32 Z = Hit.Location.Z;

					Hit.Location = FVector(X * Item->GridSize.X, Y * Item->GridSize.Y, Z * Item->GridSize.Z);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Location!: x: %f, y: %f z: %f"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z));
				}

				Hit.Location += Item->PlaceOffset;

				Item->Scene->SetWorldLocation(Hit.Location);
			}
			else
			{
				if (Item->bUseGrid && Item->GridSize.X > 0 && Item->GridSize.Y > 0 && Item->GridSize.Z > 0)
				{
					FVector NewLocation = EndTrace;

					NewLocation /= Item->GridSize;

					int32 X = NewLocation.X;
					int32 Y = NewLocation.Y;
					int32 Z = NewLocation.Z;

					NewLocation = FVector(X * Item->GridSize.X, Y * Item->GridSize.Y, Z * Item->GridSize.Z);

					NewLocation += Item->PlaceOffset;

					Item->Scene->SetWorldLocation(NewLocation);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Location!: x: %d, y: %d z: %d"), NewLocation.X, NewLocation.Y, NewLocation.Z));
				}
				else
				{
					auto NewEndTrace = EndTrace + Item->PlaceOffset;

					Item->Scene->SetWorldLocation(NewEndTrace);
				}
			}
		}
	}

	Super::Tick(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpaceStationGameCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Use", IE_Pressed, this, &ASpaceStationGameCharacter::Use);
	InputComponent->BindAction("Drop", IE_Pressed, this, &ASpaceStationGameCharacter::Drop);

	InputComponent->BindAction("Enter", IE_Pressed, this, &ASpaceStationGameCharacter::Enter);

	InputComponent->BindAction("Swap Hands", IE_Pressed, this, &ASpaceStationGameCharacter::SwapSelectedItem);
	
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASpaceStationGameCharacter::TouchStarted);
	if( EnableTouchscreenMovement(InputComponent) == false )
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASpaceStationGameCharacter::OnFire);
	}
	
	InputComponent->BindAxis("MoveForward", this, &ASpaceStationGameCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASpaceStationGameCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ASpaceStationGameCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ASpaceStationGameCharacter::LookUpAtRate);
}

void ASpaceStationGameCharacter::OnFire()
{ 
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<ASpaceStationGameProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if(FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if(AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void ASpaceStationGameCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASpaceStationGameCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ASpaceStationGameCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ASpaceStationGameCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASpaceStationGameCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASpaceStationGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpaceStationGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASpaceStationGameCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASpaceStationGameCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ASpaceStationGameCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASpaceStationGameCharacter::TouchUpdate);
	}
	return bResult;
}

void ASpaceStationGameCharacter::SwapSelectedItem_Implementation()
{
	if (SelectedItem == EHumanInventorySlot::EHumanInven_Slot_LHand)
	{
		SelectedItem = EHumanInventorySlot::EHumanInven_Slot_RHand;
	}
	else
	{
		SelectedItem = EHumanInventorySlot::EHumanInven_Slot_LHand;
	}
}

EHumanInventorySlot ASpaceStationGameCharacter::GetSelectedItem()
{
	return SelectedItem;
}

float ASpaceStationGameCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FLimbDamageEvent::ClassID))
	{
		FLimbDamageEvent* LimbDamageEvent = (FLimbDamageEvent*)&DamageEvent;
		
		LimbDamageEvent->Limb->AddLimbDamage(LimbDamageEvent->LimbDamage);
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;

		if (Cast<ULimb>(PointDamageEvent->HitInfo.GetComponent()))
		{
			FLimbDamageStruct LimbDamage;

			LimbDamage.Brute_Damage = PointDamageEvent->Damage;

			Cast<ULimb>(PointDamageEvent->HitInfo.GetComponent())->AddLimbDamage(LimbDamage);
		}
	}

	CalculateHealth(Damage, DamageEvent);

	return Damage;
}

void ASpaceStationGameCharacter::CalculateHealth(float Damage, struct FDamageEvent const& DamageEvent)
{
	if (HealthCalculationMethod == EHealthCalculationMethod::Additive)
	{
		if (DamageEvent.IsOfType(FLimbDamageEvent::ClassID))
		{
			// I still don't get why I have to use a C-style cast
			FLimbDamageEvent* LimbDamageEvent = (FLimbDamageEvent*)&DamageEvent;

			Health -= LimbDamageEvent->LimbDamage.Brute_Damage;
			Health -= LimbDamageEvent->LimbDamage.Burn_Damage;
			Health -= LimbDamageEvent->LimbDamage.Suffocation_Damage;
			Health -= LimbDamageEvent->LimbDamage.Toxin_Damage;
		}
		else
		{
			Health -= Damage;
		}
	}
	else if (HealthCalculationMethod == EHealthCalculationMethod::Realistic)
	{
		//welp i'll do this shit later
	}
}

void ASpaceStationGameCharacter::Use_Implementation()
{
		//Line trace for usable objects.
		FVector CamLoc;
		FRotator CamRot;

		GetController()->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
		const FVector StartTrace = CamLoc; // trace start is the camera location
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + Direction * MaxUseDistance; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

		// Perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams); // simple trace function

		//Draw debug line
		//Keep the code here in case I need it later :)

		//DrawDebugLine(
		//	GetWorld(),
		//	StartTrace,
		//	EndTrace,
		//	FColor(255, 0, 0),
		//	true, -1, 0,
		//	12.333
		//	);

		if (Cast<AItem>(Hit.GetActor()))
		{
			Cast<AItem>(Hit.GetActor())->Use(this);
		}
}

void ASpaceStationGameCharacter::Drop()
{
	if (InventoryItemIsValid(GetSelectedItem()))
	{
		AItem* Item = GetInventoryItem(GetSelectedItem());

		if (Item->bUseHologram)
		{
			if (!Item->bBeingDropped)
			{
				Item->CustomDepthMesh->RegisterComponent();

				Item->CustomDepthMesh->SetRenderCustomDepth(true);

				Item->bBeingDropped = true;

				if (Cast<ASpaceStationGamePlayerController>(GetController()))
				{
					Cast<ASpaceStationGamePlayerController>(GetController())->bHighlightUsableItems = false;
				}
			}
			else
			{
				Item->bBeingDropped = false;

				Item->CustomDepthMesh->SetRelativeLocation(FVector(0, 0, 0), false);

				if (Cast<ASpaceStationGamePlayerController>(GetController()))
				{
					Cast<ASpaceStationGamePlayerController>(GetController())->bHighlightUsableItems = true;
				}

				Drop_Server();
			}
		}
		else
		{
			Drop_Server();
		}
	}
}

void ASpaceStationGameCharacter::Drop_Server_Implementation()
{
	if (InventoryItemIsValid(GetSelectedItem()))
	{
		GetInventoryItem(GetSelectedItem())->Drop(this);
	}
}

bool ASpaceStationGameCharacter::InventoryItemIsValid(EHumanInventorySlot Index)
{
	return InventoryStruct.Index.Contains((uint8)Index);
}

AItem* ASpaceStationGameCharacter::GetInventoryItem(EHumanInventorySlot Index) 
{ 
	int32 NewIndex = InventoryStruct.Index.IndexOfByKey((uint8)Index);

	return InventoryStruct.Items[NewIndex];
}

uint8 ASpaceStationGameCharacter::GetInventoryItemByValue(AItem* Item)
{
	int32 NewIndex = InventoryStruct.Items.Find(Item);

	return InventoryStruct.Index[NewIndex];
}

void ASpaceStationGameCharacter::SwapInventoryItem(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2)
{
	if (!InventoryStruct.Index.FindByKey((uint8)Slot1) && !InventoryStruct.Index.FindByKey((uint8)Slot2))
	{
		return;
	}

	if (InventoryStruct.Index.FindByKey((uint8)Slot1) && InventoryStruct.Index.FindByKey((uint8)Slot2))
	{
		int32 Slot1Index = InventoryStruct.Index.IndexOfByKey((uint8)Slot1);
		int32 Slot2Index = InventoryStruct.Index.IndexOfByKey((uint8)Slot2);

		InventoryStruct.Index[Slot1Index] = (uint8)Slot2;

		InventoryStruct.Index[Slot2Index] = (uint8)Slot1;

		SwapInventoryItem_Server(Slot1, Slot2);

		return;
	}

	// if only one slot is valid, give the index to the other slot
	else if (InventoryStruct.Index.FindByKey((uint8)Slot1))
	{
		InventoryStruct.Index[InventoryStruct.Index.IndexOfByKey((uint8)Slot1)] = (uint8)Slot2;

		SwapInventoryItem_Server(Slot1, Slot2);

		return;
	}
	else if (InventoryStruct.Index.FindByKey((uint8)Slot2))
	{
		InventoryStruct.Index[InventoryStruct.Index.IndexOfByKey((uint8)Slot2)] = (uint8)Slot1;

		SwapInventoryItem_Server(Slot1, Slot2);

		return;
	}
}

void ASpaceStationGameCharacter::SwapInventoryItem_Server_Implementation(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2)
{
	// if neither slots are valid, return
	if (!InventoryStruct.Index.FindByKey((uint8)Slot1) && !InventoryStruct.Index.FindByKey((uint8)Slot2) || Slot1 == Slot2)
	{
		return;
	}

	// if both slots are valid, swap them
	if (InventoryStruct.Index.FindByKey((uint8)Slot1) && InventoryStruct.Index.FindByKey((uint8)Slot2))
	{
		int32 Slot1Index = InventoryStruct.Index.IndexOfByKey((uint8)Slot1);
		int32 Slot2Index = InventoryStruct.Index.IndexOfByKey((uint8)Slot2);

		InventoryStruct.Index[Slot1Index] = (uint8)Slot2;

		InventoryStruct.Index[Slot2Index] = (uint8)Slot1;

		return;
	}
	// if only one slot is valid, give the index to the other slot
	else if (InventoryStruct.Index.FindByKey((uint8)Slot1))
	{
		InventoryStruct.Index[InventoryStruct.Index.IndexOfByKey((uint8)Slot1)] = (uint8)Slot2;

		return;
	}
	else if (InventoryStruct.Index.FindByKey((uint8)Slot2))
	{
		InventoryStruct.Index[InventoryStruct.Index.IndexOfByKey((uint8)Slot2)] = (uint8)Slot1;

		return;
	}
}

void ASpaceStationGameCharacter::AddInventoryItemIndex_Implementation(AItem* Item, EHumanInventorySlot Index)
{
	InventoryStruct.Index.Add((uint8)Index);
	InventoryStruct.Items.Add(Item);

	//InventoryMap.Add(Index, Item);
}

void ASpaceStationGameCharacter::RemoveInventoryItem_Implementation(EHumanInventorySlot Index)
{
	int32 NewIndex = InventoryStruct.Index.IndexOfByKey((uint8)Index);
	
	InventoryStruct.Index.RemoveAt(NewIndex);
	InventoryStruct.Items.RemoveAt(NewIndex);



	//InventoryMap.Remove(Index);
}

float ASpaceStationGameCharacter::GetHealth()
{
	return Health;
}

void ASpaceStationGameCharacter::SetHealth_Implementation(float NewHealth)
{
	Health = NewHealth;
}

FString ASpaceStationGameCharacter::GetPawnName()
{
	return PawnName;
}

void ASpaceStationGameCharacter::SetPawnName_Implementation(const FString& NewName)
{
	PawnName = NewName;
}

void ASpaceStationGameCharacter::SetAffectedItem_Implementation(AItem* Item)
{
	AffectedItem = Item;
}

AItem* ASpaceStationGameCharacter::GetAffectedItem()
{
	return AffectedItem;
}

void ASpaceStationGameCharacter::Enter()
{
	Cast<ASpaceStationGamePlayerController>(GetController())->Enter();
}

void ASpaceStationGameCharacter::AddReagent_Implementation(EReagents Reagent, int32 Amount)
{
	if (Cast<AChemDispenser>(AffectedItem))
	{
		Cast<AChemDispenser>(AffectedItem)->AddReagent(Reagent, Amount);
	}
}