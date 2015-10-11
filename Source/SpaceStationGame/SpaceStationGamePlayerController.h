// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Item.h"
#include "IDCard.h"
#include "Jobs.h"
#include "ChatMessageStruct.h"
#include "SpaceStationGamePlayerController.generated.h"

/**
*
*/
UCLASS()
class SPACESTATIONGAME_API ASpaceStationGamePlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	void ResetOutline();

	FTimerHandle OutlineTimerHandle;

	TWeakObjectPtr<AItem> OutlineActor;

	TWeakObjectPtr<AIDCard> CardActor;

	UFUNCTION(exec)
		void ChatFunc(const FString& InputString);

	UFUNCTION(reliable, server, WithValidation)
		void ServerChatFunc(const FString& InputString);

	void ServerChatFunc_Implementation(const FString& InputString);

	bool ServerChatFunc_Validate(const FString& InputString) { return true; };

	TArray<FClientChatMessageStruct> ChatMessages;

	FString GetChatName();

public:

	ASpaceStationGamePlayerController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, server, unreliable, withvalidation, Category = Round)
		void ToggleRoundStartReady();

	bool ToggleRoundStartReady_Validate() { return true; };

	void ToggleRoundStartReady_Implementation();

	UFUNCTION(BlueprintCallable, server, unreliable, withvalidation, Category = Round)
		void JoinGame();

	bool JoinGame_Validate() { return true; };

	void JoinGame_Implementation();

	UFUNCTION(Client, Reliable)
		void HandleRoundStart();

	void HandleRoundStart_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = Default)
		void HandleRoundStart_BP();

	bool bHighlightUsableItems;

	/** True if the network time is valid. */
	bool IsNetworkTimeValid();

	UFUNCTION(client, reliable, withvalidation)
		void AddChatMessage(const FString& Msg, bool Radio);

	bool AddChatMessage_Validate(const FString& Msg, bool Radio) { return true; };

	void AddChatMessage_Implementation(const FString& Msg, bool Radio);

	UFUNCTION(BlueprintImplementableEvent, Category = Default)
		void AddChatMessageHud(FClientChatMessageStruct Msg);

	/** Gets the current system time in milliseconds */
	static int64 GetLocalTime();

	/** Gets the approximate current network time in milliseconds. */
	int64 GetNetworkTime();

	UFUNCTION(BlueprintCallable, Category = Chat)
		TArray<FClientChatMessageStruct> GetChatMessages();

	UFUNCTION(BlueprintImplementableEvent, Category = UI)
		void Enter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Replication)
		EJobs StartingJob;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Round)
		bool RoundStartReady;

	UFUNCTION(BlueprintCallable, Server, WithValidation, Unreliable, Category = Job)
		void SetStartingJob(EJobs Job);

	bool SetStartingJob_Validate(EJobs Job) { return true; };

	void SetStartingJob_Implementation(EJobs Job);

protected:
	int64 timeServerTimeRequestWasPlaced;
	int64 timeOffsetFromServer;
	bool timeOffsetIsValid;

	/** Sent from a client to the server to get the server's system time */
	UFUNCTION(reliable, server, WithValidation)
		void ServerGetServerTime();

	void ServerGetServerTime_Implementation();

	bool ServerGetServerTime_Validate() { return true; };

	/** Sent from the server to a client to give them the server's system time */
	UFUNCTION(reliable, client)
		void ClientGetServerTime(int64 serverTime);

	void ClientGetServerTime_Implementation(int64 serverTime);
};
