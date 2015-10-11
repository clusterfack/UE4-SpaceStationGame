#pragma once

#include "SpaceStationGameHUD.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SlateGameResources.h"

class FSpaceStationGameUIResources
{
public:
	FSpaceStationGameUIResources() : m_Path("/Game/UI"), SpaceStationGameUIResources(NULL) { } ;

	/*Loads resources and registers them with Slate*/
	/*Do this before trying to use resources*/
	void Initialize();

	/*cleanup*/
	/*Do this when shutting down game module*/
	void Shutdown();

	/*reloads textures used by slate renderer*/
	/*Does nothing at the moment*/
	void ReloadTextures();

	/*Give caller a pointer to our FSlateGameResources*/
	TSharedPtr<FSlateGameResources> GetSlateGameResources();

protected:
	/*Creates resources*/
	TSharedRef<class FSlateGameResources> Create();

	/*Defined in Cpp file, change as needed*/
	const FString m_Path;

	/*Poitner to game resources, Initialize() before using*/
	TSharedPtr<FSlateGameResources> SpaceStationGameUIResources;
};