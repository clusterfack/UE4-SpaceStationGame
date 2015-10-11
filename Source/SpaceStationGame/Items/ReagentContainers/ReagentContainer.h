// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Reagents.h"
#include <map>
#include <unordered_map>
#include "RecipeContainer.h"

#include "ReagentContainer.generated.h"

UCLASS()
class SPACESTATIONGAME_API AReagentContainer : public AItem
{
	GENERATED_BODY()

	FRecipe Recipe;

	void Explode(uint32 ExplosionSize);

public:
	AReagentContainer(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRep Update Amount Function"), Category = UI)
		virtual void UIUpdateAmount(EReagents Reagent, int32 Amnt);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Ingredients)
		void AddReagent(EReagents Reagent, int32 Amount);

	void AddReagent_Implementation(EReagents Reagent, int32 Amount);

	bool AddReagent_Validate(EReagents Reagent, int32 Amount) { return true; };

	UFUNCTION(BlueprintImplementableEvent, Category = Use)
		void UseReagentContainer(APawn* Pawn);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UseReagentContainerOnChemDispenser(FVector NewMeshLocation);

	bool UseReagentContainerOnChemDispenser_Validate(FVector NewMeshLocation) { return true; };

	void UseReagentContainerOnChemDispenser_Implementation(FVector NewMeshLocation);

	UFUNCTION(Client, Reliable, WithValidation)
		void UseReagentContainerOnChemDispenser_Client(FVector NewMeshLocation);

	bool UseReagentContainerOnChemDispenser_Client_Validate(FVector NewMeshLocation) { return true; };

	UFUNCTION(BlueprintImplementableEvent, Category = Use)
		void UseReagentContainerOnChemDispenser_Client_Implementation(FVector NewMeshLocation);

	UFUNCTION(Client, Reliable, WithValidation)
		void UpdateReagentsOnUse();

	bool UpdateReagentsOnUse_Validate() { return true; };

	void UpdateReagentsOnUse_Implementation();

	UFUNCTION(BlueprintCallable, Category = Ingredients)
	uint8 GetReagentEnumInt(EReagents Reagent);
};
