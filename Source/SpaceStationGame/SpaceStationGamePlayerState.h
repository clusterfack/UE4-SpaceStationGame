// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SpaceStationGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SPACESTATIONGAME_API ASpaceStationGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASpaceStationGamePlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void Tick( float DeltaTime ) override;

	virtual void BeginPlay() override;
};
