#pragma once

#include "BloodTypeStruct.generated.h"

// This struct denotes the presence of blood antigens
// For example, Antigen_A = true, Antigen_B = true, Antigen_RH = true would denote bloodtype AB+

USTRUCT(BlueprintType)
struct FBloodTypeStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		bool Antigen_A;

	UPROPERTY()
		bool Antigen_B;

	UPROPERTY()
		bool Antigen_RH;
};