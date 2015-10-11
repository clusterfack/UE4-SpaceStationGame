// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "UnrealNetwork.h"
#include "SpaceStationGamePlayerState.h"

ASpaceStationGamePlayerState::ASpaceStationGamePlayerState(const FObjectInitializer& ObjectInitializer)
{
	//PrimaryActorTick.bCanEverTick = true;
}


void ASpaceStationGamePlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//FString SteamID = GetWorld()->GetFirstPlayerController()->PlayerState->UniqueId->ToString();
}

void ASpaceStationGamePlayerState::BeginPlay()
{
	Super::BeginPlay();
}