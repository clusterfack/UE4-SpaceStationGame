// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "SpaceStationGameCharacter.h"
#include "Reagents.h"
#include "ReagentContainer.h"
#include "Inventory.h"
#include "UnrealNetwork.h"
#include "ChemDispenser.h"

AChemDispenser::AChemDispenser(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ReagentAttachmentPoint = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Reagent Container Attachment Point"));

	ReagentAttachmentPoint->AttachTo(RootComponent, NAME_None, EAttachLocation::KeepRelativeOffset);

	ReagentAttachmentPoint->bAbsoluteLocation = false;
	ReagentAttachmentPoint->bAbsoluteRotation = false;
	ReagentAttachmentPoint->bAbsoluteScale = true;

	bReplicates = true;
	bReplicateMovement = true;
}

void AChemDispenser::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChemDispenser, ReagentContainer);
}

void AChemDispenser::Use_Implementation(APawn* Pawn)
{
	if (Cast<ASpaceStationGameCharacter>(Pawn))
	{
		ASpaceStationGameCharacter* SpaceStationGameCharacter = Cast<ASpaceStationGameCharacter>(Pawn);
		
		if (SpaceStationGameCharacter->InventoryItemIsValid(SpaceStationGameCharacter->GetSelectedItem()) && Cast<AReagentContainer>(SpaceStationGameCharacter->GetInventoryItem(SpaceStationGameCharacter->GetSelectedItem())))
		{
			ReagentContainer = Cast<AReagentContainer>(SpaceStationGameCharacter->GetInventoryItem(SpaceStationGameCharacter->GetSelectedItem()));

			ReagentContainer->SetOwner(Pawn);

			ReagentContainer->UseReagentContainerOnChemDispenser(ReagentAttachmentPoint->GetComponentLocation());

			uint8 Index = SpaceStationGameCharacter->GetInventoryItemByValue(ReagentContainer);

			SpaceStationGameCharacter->SetAffectedItem(this);

			SpaceStationGameCharacter->RemoveInventoryItem((EHumanInventorySlot)Index);
		}
	}
}

void AChemDispenser::AddReagent_Implementation(EReagents Reagent, int32 Amount)
{
	//Sanitize this shit, blueprints don't support uint32 so we gotta make do
	if (Amount < 0)
	{
		Amount = 0;
	}

	if (ReagentContainer->IsValidLowLevel())
	{
		ReagentContainer->AddReagent(Reagent, Amount);
	}
}

AReagentContainer* AChemDispenser::GetReagentContainer()
{
	return ReagentContainer;
}
