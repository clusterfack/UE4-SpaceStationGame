// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __SPACESTATIONGAME_H__
#define __SPACESTATIONGAME_H__

#pragma once

#include "Engine.h"

#include "SpaceStationGameGameResources.h"

DECLARE_LOG_CATEGORY_EXTERN(SpaceStationGameLog, Log, All);

class FSpaceStationGame : public FDefaultGameModuleImpl
{
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Called when GameModule is loaded, load any resources game may need here
	void StartupModule();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Called when GameModule is unloaded, before shutdown, unload resources/cleanup here
	void ShutdownModule();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Give a handle to MyUIResources to anyone who asks
	TSharedPtr<FSlateGameResources> GetSlateGameResources();

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Data Structure and Interface for maintaining SlateGameResources on Game to Game basis
	FSpaceStationGameUIResources SpaceStationGameUIResources;

};

#endif
