// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SpaceStationGame.h"

DEFINE_LOG_CATEGORY(SpaceStationGameLog);

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SpaceStationGame, "SpaceStationGame" );
 
void FSpaceStationGame::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	/*Loads resources and registers them with Slate*/
	/*Do this before trying to use resources*/
	SpaceStationGameUIResources.Initialize();

	/*TODO: Anything else game module might need to do on load*/
}

void FSpaceStationGame::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();

	/*Unregister resources/styles with Slate, cleanup, free memory*/
	SpaceStationGameUIResources.Shutdown();

	/*Cleanup/free any resources here*/
}

/*First defined here, no need to call parent*/
/*Give caller a pointer to our FSlateGameResources*/
TSharedPtr<FSlateGameResources> FSpaceStationGame::GetSlateGameResources()
{
	/*Give caller a pointer to our FSlateGameResources*/
	/*Giving strong pointer, helps gurantee access to resources*/
	return SpaceStationGameUIResources.GetSlateGameResources();
}