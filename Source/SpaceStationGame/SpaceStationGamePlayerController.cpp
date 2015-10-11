// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "SpaceStationGameGameState.h"
#include "SpaceStationGameCharacter.h"
#include "SpaceStationGamePlayerController.h"
#include "Item.h"
#include "IDCard.h"
#include "ChatMessageStruct.h"
#include "UnrealNetwork.h"
#include <chrono>

using namespace std::chrono;

ASpaceStationGamePlayerController::ASpaceStationGamePlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bHighlightUsableItems = true;
}

void ASpaceStationGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Ask the server for its current time
	if (Role < ROLE_Authority)
	{
		timeServerTimeRequestWasPlaced = GetLocalTime();
		ServerGetServerTime();
	}
}

void ASpaceStationGamePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role < ROLE_Authority && Cast<ASpaceStationGameCharacter>(GetPawn()) && bHighlightUsableItems)
	{
		//Tick function that causes the outline around usable objects

		FVector CamLoc;
		FRotator CamRot;

		GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
		const FVector StartTrace = CamLoc; // trace start is the camera location
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + Direction * Cast<ASpaceStationGameCharacter>(GetPawn())->MaxUseDistance; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

		// Perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(FName(TEXT("Use Trace")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams); // simple trace function

		if (Cast<AItem>(Hit.GetActor()))
		{
			// Turn off the outline on the previous object
			if (Cast<AItem>(Hit.GetActor()) != OutlineActor)
			{
				ResetOutline();
			}

			// Turn on the outline on the (new) object
			Cast<AItem>(Hit.GetActor())->CustomDepthMesh->SetRenderCustomDepth(true);

			// Set a timer to turn off the outline
			OutlineActor = Cast<AItem>(Hit.GetActor());
			GetWorldTimerManager().SetTimer(OutlineTimerHandle, this, &ASpaceStationGamePlayerController::ResetOutline, 0.1f, false);
		}
	}
}

void ASpaceStationGamePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceStationGamePlayerController, StartingJob);
	DOREPLIFETIME_CONDITION(ASpaceStationGamePlayerController, RoundStartReady, COND_OwnerOnly);
}

void ASpaceStationGamePlayerController::ToggleRoundStartReady_Implementation()
{
	RoundStartReady = !RoundStartReady;
}

void ASpaceStationGamePlayerController::JoinGame_Implementation()
{
	RoundStartReady = true;

	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void ASpaceStationGamePlayerController::SetStartingJob_Implementation(EJobs Job)
{
	StartingJob = Job;
}

void ASpaceStationGamePlayerController::ResetOutline()
{
	if (OutlineActor.IsValid())
	{
		OutlineActor->CustomDepthMesh->SetRenderCustomDepth(false);
	}
}

void ASpaceStationGamePlayerController::HandleRoundStart_Implementation()
{
	HandleRoundStart_BP();
}

/////////////////////////////////////////////////
//Networking stuff
//Handles the data for object position interpolation

/** Gets the current system time in milliseconds */
/* static */ int64 ASpaceStationGamePlayerController::GetLocalTime()
{
	milliseconds ms = duration_cast< milliseconds >(high_resolution_clock::now().time_since_epoch());
	return (int64)ms.count();
}

/** Sent from a client to the server to get the server's system time */
void ASpaceStationGamePlayerController::ServerGetServerTime_Implementation()
{
	ClientGetServerTime(GetLocalTime());
}

/** True if the network time is valid. */
bool ASpaceStationGamePlayerController::IsNetworkTimeValid()
{
	return timeOffsetIsValid;
}

/** Sent from the server to a client to give them the server's system time */
void ASpaceStationGamePlayerController::ClientGetServerTime_Implementation(int64 serverTime)
{
	int64 localTime = GetLocalTime();

	// Calculate the server's system time at the moment we actually sent the request for it.
	int64 roundTripTime = localTime - timeServerTimeRequestWasPlaced;
	serverTime -= roundTripTime / 2;

	// Now calculate the difference between the two values
	timeOffsetFromServer = serverTime - timeServerTimeRequestWasPlaced;

	// Now we can safely say that the following is true
	//
	// serverTime = timeServerTimeRequestWasPlaced + timeOffsetFromServer
	//
	// which is another way of saying
	//
	// NetworkTime = LocalTime + timeOffsetFromServer

	timeOffsetIsValid = true;
}

/** Gets the approximate current network time in milliseconds. */
int64 ASpaceStationGamePlayerController::GetNetworkTime()
{
	return GetLocalTime() + timeOffsetFromServer;
}

void ASpaceStationGamePlayerController::ChatFunc(const FString& InputString)
{
	FString Command;
	FString Arguments;

	FString InputStringIterator = InputString; //*Iter;

	if (InputStringIterator.Find(TEXT("(")) > 0 && InputStringIterator.Find(TEXT(")")) > 0 && InputStringIterator.Find(TEXT("(")) < InputStringIterator.Find(TEXT(")")))
	{
		Command = InputStringIterator.Left(InputStringIterator.Find(TEXT("(")));
		Command = Command.Trim();

		Arguments = InputStringIterator.Mid(InputStringIterator.Find(TEXT("(")) + 1, (InputStringIterator.Find(TEXT(")")) - InputStringIterator.Find(TEXT("(")) - 1));

		if (Command.Equals(TEXT("Say"), ESearchCase::IgnoreCase))
		{
			if (!Arguments.IsEmpty())
			{
				ServerChatFunc(InputStringIterator.Left(128));
			}
		}
	}
	else
	{
		// Don't waste bandwidth if we dont have to ;^)

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unrecognized Command"));
	}
}

void ASpaceStationGamePlayerController::ServerChatFunc_Implementation(const FString& InputString)
{
	FString Command;
	FString Arguments;

	FString InputStringIterator = InputString; //*Iter;

	if (InputStringIterator.Find(TEXT("(")) > 0 && InputStringIterator.Find(TEXT(")")) > 0 && InputStringIterator.Find(TEXT("(")) < InputStringIterator.Find(TEXT(")")))
	{
		Command = InputStringIterator.Left(InputStringIterator.Find(TEXT("(")));
		Command = Command.Trim();

		Arguments = InputStringIterator.Mid(InputStringIterator.Find(TEXT("(")) + 1, (InputStringIterator.Find(TEXT(")")) - InputStringIterator.Find(TEXT("(")) - 1));

		if (Command.Equals(TEXT("Say"), ESearchCase::IgnoreCase))
		{
			if (!Arguments.IsEmpty())
			{
				Cast<ASpaceStationGameGameState>(GetWorld()->GetGameState())->AddChatMessage(Arguments.Left(128), GetPawn()->GetActorLocation(), GetChatName());
			}
		}
	}
}

FString ASpaceStationGamePlayerController::GetChatName()
{
	if (Cast<ASpaceStationGameCharacter>(GetPawn()))
	{
		return Cast<ASpaceStationGameCharacter>(GetPawn())->GetPawnName();
	}
	else
	{
		return TEXT("Spectator");
	}
}

void ASpaceStationGamePlayerController::AddChatMessage_Implementation(const FString& Msg, bool Radio)
{
	FClientChatMessageStruct NewMessage;
	if (Radio)
	{
		NewMessage.MessageColor = FColor::Red;
	}
	else
	{
		NewMessage.MessageColor = FColor::Black;
	}

	NewMessage.ChatMsg = Msg;
	NewMessage.MessageColor = FColor::Black;


	ChatMessages.Add(NewMessage);

	AddChatMessageHud(NewMessage);
}

TArray<FClientChatMessageStruct> ASpaceStationGamePlayerController::GetChatMessages()
{
	return ChatMessages;
}

//void ASpaceStationGamePlayerController::Enter()
//{
//
//}