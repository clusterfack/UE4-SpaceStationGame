// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "Item.h"
#include "SpaceStationGameGameResources.h"
#include "SpaceStationGameHUD.generated.h"

UCLASS()
class ASpaceStationGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASpaceStationGameHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

protected:
};

