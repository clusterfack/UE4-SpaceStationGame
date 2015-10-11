#pragma once

#include "Reagents.generated.h"

UENUM(BlueprintType)
enum class EReagents : uint8
{
	EChem_Null			UMETA(DisplayName = "NULL CHEMICAL"),
	// This chemical creates an explosion upon initializing
	EChem_Explosion		UMETA(DisplayName = "Explosion Chemical"),
	EChem_Oxygen		UMETA(DisplayName = "Oxygen"),
	EChem_Water			UMETA(DisplayName = "Water"),
	EChem_Nitrogen 		UMETA(DisplayName = "Nitrogen"),
	EChem_Potassium 	UMETA(DisplayName = "Potassium"),
	EChem_Silicon		UMETA(DisplayName = "Silicon"),
	EChem_Carbon		UMETA(DisplayName = "Carbon"),
	EChem_Hydrogen		UMETA(DisplayName = "Hydrogen")
};