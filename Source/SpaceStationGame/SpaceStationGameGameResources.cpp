// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SpaceStationGame.h"
#include "SpaceStationGameGameResources.h"
#include "SpaceStationGameCharacter.h"
#include "SlateGameResources.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*FMyUIResources*//*FMyUIResources*//*FMyUIResources*//*FMyUIResources*//*FMyUIResources*/
////////////////////////////////////////////////////////////////////////////////////////////////////

void FSpaceStationGameUIResources::Initialize()
{
	if (!SpaceStationGameUIResources.IsValid())
	{
		SpaceStationGameUIResources = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*SpaceStationGameUIResources);
	}
}

TSharedPtr<FSlateGameResources> FSpaceStationGameUIResources::GetSlateGameResources()
{
	return SpaceStationGameUIResources;
}

TSharedRef<class FSlateGameResources> FSpaceStationGameUIResources::Create()
{
	return FSlateGameResources::New(FName("SpaceStationGameUIResources"), m_Path, m_Path);
}

/*Unregister resources/styles with Slate, cleanup, free memory*/
void FSpaceStationGameUIResources::Shutdown()
{
	//Unregister *MyUIResources with Slate
	FSlateStyleRegistry::UnRegisterSlateStyle(*SpaceStationGameUIResources);

	//Debugging
	ensure(SpaceStationGameUIResources.IsUnique());

	//Removes reference to resources, decrements refcount, destroys resources if refcount=0
	//Do this to all SharedPtrs on Shutdown() or SomethingSimilar() to avoid memory leak
	SpaceStationGameUIResources.Reset();
}
