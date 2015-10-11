// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "IDCard.h"
#include "Jobs.h"

FJobStruct FJobCaptain::FJobStructConstruct(APawn* Owner)
{
	UWorld* const World = Owner->GetWorld();

	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;

		AIDCard* IDCard = World->SpawnActor<AIDCard>(AIDCard::StaticClass(), SpawnParams);

		if (Cast<ASpaceStationGameCharacter>(Owner))
		{
			IDCard->ServerSetAssignedName(Cast<ASpaceStationGameCharacter>(Owner)->GetPawnName());

			IDCard->ServerSetJob(EJobs::EJob_Captain);
		}

		StartingInventory.Items.Add(IDCard);
		StartingInventory.Index.Add((uint8)EHumanInventorySlot::EHumanInven_Slot_ID);
	}

	return *this;
};