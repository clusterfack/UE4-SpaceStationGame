#pragma once

#include "Item.h"
#include "Inventory.generated.h"

UENUM(BlueprintType)
enum class EHumanInventorySlot : uint8
{
	EHumanInven_Slot_LHand			UMETA(DisplayName = "Left Hand"),
	EHumanInven_Slot_RHand			UMETA(DisplayName = "Right Hand"),
	EHumanInven_Slot_Back			UMETA(DisplayName = "Back"),
	EHumanInven_Slot_ID				UMETA(DisplayName = "ID Slot"),
	EHumanInven_Slot_Pocket1		UMETA(DisplayName = "Pocket Slot 1"),
	EHumanInven_Slot_Pocket2		UMETA(DisplayName = "Pocket Slot 2"),
	EHumanInven_Slot_Belt			UMETA(DisplayName = "Belt Slot"),
	EHumanInven_Slot_SuitStorage	UMETA(DisplayName = "Suit Storage")
};

USTRUCT()
struct FInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<AItem*> Items;

	UPROPERTY()
		TArray<uint8> Index;

};