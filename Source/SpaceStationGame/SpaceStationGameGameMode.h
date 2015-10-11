// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "SpaceStationGameGameMode.generated.h"

UCLASS(minimalapi)
class ASpaceStationGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASpaceStationGameGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Round)
		float RoundStartDelay;

	bool bUseRoundStartReady;

	virtual void StartRound();

	virtual void HandleMatchHasStarted() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void RestartPlayer(class AController* NewPlayer) override;
};
