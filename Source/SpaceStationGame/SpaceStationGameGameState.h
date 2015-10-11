// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RecipeContainer.h"
#include "Reagents.h"
#include <map>
#include <unordered_map>
#include "GameFramework/GameState.h"
#include "ChatMessageStruct.h"
#include "InstancedItemContainer.h"
#include "SpaceStationGameGameState.generated.h"

/**
*
*/
UCLASS()
class ASpaceStationGameGameState : public AGameState
{
	GENERATED_BODY()

	TArray<FChatMessageStruct*> ChatMessages;

	UFUNCTION(Client, Reliable, WithValidation)
		void SendNewChatMessage(const FString& Msg, const FString& PlayerName, FVector Location);

	bool SendNewChatMessage_Validate(const FString& Msg, const FString& PlayerName, FVector Location) { return true; };

	void SendNewChatMessage_Implementation(const FString& Msg, const FString& PlayerName, FVector Location);

	UDataTable* RecipeLookupTable;

	std::pair<EReagents, uint32> GetReagentPair(FString Reagent, FString StringToInt);

	void SetupRecipes();

	FTimerHandle RoundStartTimerHandle;

	UPROPERTY()
		class UMySQLObject* MySQLObject;

public:
	ASpaceStationGameGameState(const FObjectInitializer& ObjectInitializer);

	void StartMatchTimer(float TimerLength);

	void StartRound();

	void SetUpMySQLPlayerData(APlayerController* NewPlayer);

	uint8 GetMySQLPreferredJob(APlayerController* NewPlayer);

	UPROPERTY()
		URecipeContainer* RecipeContainer;

	UPROPERTY(Replicated)
		TArray<AInstancedItemContainer*> InstancedItemContainers;

	UPROPERTY(Replicated)
		bool bDelayedStart;

	UFUNCTION(BlueprintCallable, Category = Round)
	bool GetDelayedStart() { return bDelayedStart; };

	/*UPROPERTY(Replicated)
		TArray<FItemContainerStruct> ItemContainerStructs;*/

	UPROPERTY(EditAnywhere, Category = Item)
		TArray<TSubclassOf<class AInstancedItemContainer>> InstancedItemContainerClasses;

	virtual void BeginPlay() override;

	AInstancedItemContainer* GetContainerFromClass(UClass* InputClass);

	bool GetRecipeExists(FRecipe InRecipe);

	FRecipe GetProductReagentFromRecipe(FRecipe InRecipe);

	void AddChatMessage(const FString& Msg, FVector PlayerLocation, const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = State, meta = (DisplayName = "Get Match State"))
		FName BP_GetMatchState() { return GetMatchState(); };

};

USTRUCT(Blueprintable)
struct FRecipeLookupTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "RO")
		FString Reagents;

	UPROPERTY(BlueprintReadOnly, Category = "RO")
		FString Products;
};