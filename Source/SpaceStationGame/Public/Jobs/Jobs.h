#pragma once

#include "SpaceStationGameCharacter.h"
#include "Engine.h"
#include "Inventory.h"
#include "Jobs.generated.h"

UENUM(BlueprintType)
enum class EJobs : uint8
{
	EJob_Captain			UMETA(DisplayName = "Captain"),					//0
	EJob_HOP				UMETA(DisplayName = "Head of Personnel"),		//1

	EJob_HOS				UMETA(DisplayName = "Head of Security"),		//2
	EJob_Warden				UMETA(DisplayName = "Warden"),					//3
	EJob_Security			UMETA(DisplayName = "Security Officer"),		//4
	EJob_Detective			UMETA(DisplayName = "Detective"),				//5

	EJob_RD					UMETA(DisplayName = "Research Director"),		//6
	EJob_MD					UMETA(DisplayName = "Medical Doctor"),			//7
	EJob_Scientist			UMETA(DisplayName = "Scientist"),				//8
	EJob_Chemist			UMETA(DisplayName = "Chemist"),					//9
	EJob_Geneticist			UMETA(DisplayName = "Geneticist"),				//10
	EJob_Roboticist			UMETA(DisplayName = "Roboticist"),				//11
	EJob_Virologist			UMETA(DisplayName = "Virologist"),

	EJob_CE					UMETA(DisplayName = "Chief Engineer"),			//12
	EJob_Engineer			UMETA(DisplayName = "Station Engineer"),		//13
	EJob_AT					UMETA(DisplayName = "Atmospheric Technician"),	//14

	EJob_QM					UMETA(DisplayName = "Quarter Master"),			//15
	EJob_CargoTech			UMETA(DisplayName = "Cargo Technician"),		//16
	EJob_Miner				UMETA(DisplayName = "Shaft Miner"),				//17

	EJob_Assistant			UMETA(DisplayName = "Assistant"),				//18
	EJob_Clown				UMETA(DisplayName = "Clown"),					//19
	EJob_Mime				UMETA(DisplayName = "Mime"),					//20
	EJob_Lawyer				UMETA(DisplayName = "Lawyer"),					//21
	EJob_Janitor			UMETA(DisplayName = "Janitor"),					//22
	EJob_Chaplain			UMETA(DisplayName = "Chaplain"),				//23
	EJob_Librarian			UMETA(DisplayName = "Librarian"),				//24

	EJob_Chef				UMETA(DisplayName = "Chef"),					//25
	EJob_Bartender			UMETA(DisplayName = "Bartender"),				//26
	EJob_Botanist			UMETA(DisplayName = "Botanist")					//27
};

USTRUCT(BlueprintType)
struct FAccess
{
	GENERATED_USTRUCT_BODY()

	//Security
	uint32 bBrig : 1;
	uint32 bWeaponPermit : 1;
	uint32 bSecurity : 1;
	uint32 bHoldingCells : 1;
	uint32 bArmory : 1;
	uint32 bForensics : 1;
	uint32 bCourtroom : 1;
	uint32 bHeadOfSecurity : 1;

	//Medical
	uint32 bMedical : 1;


	//General
	uint32 bKitchen : 1;
	uint32 bBar : 1;
	uint32 bHydroponics : 1;
	uint32 bCustodialCloset : 1;
	uint32 bChapelOffice : 1;
	uint32 bCrematorium : 1;
	uint32 bLibrary : 1;
	uint32 bTheatre : 1;
	uint32 bLawOffice : 1;
	uint32 bHonkAccess : 1;
	uint32 bSilentAccess : 1;
	uint32 bMaintenanceAccess : 1;
};

USTRUCT(BlueprintType)
struct FJobStruct
{
	GENERATED_USTRUCT_BODY()

	FInventory StartingInventory;

	FAccess DefaultAccess;

public:
	virtual FJobStruct FJobStructConstruct(APawn* Owner) { return *this; };
};

USTRUCT(BlueprintType)
struct FJobCaptain : public FJobStruct
{
	GENERATED_USTRUCT_BODY()

public:
	virtual FJobStruct FJobStructConstruct(APawn* Owner) override;

};