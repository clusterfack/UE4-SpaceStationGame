// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Reagents.h"
#include "ReagentContainer.h"
//#include "SceneComponent.h"
#include "ChemDispenser.generated.h"

/**
 * 
 */
UCLASS()
class SPACESTATIONGAME_API AChemDispenser : public AItem
{
	GENERATED_BODY()

public:
	AChemDispenser(const FObjectInitializer& ObjectInitializer);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	virtual void Use_Implementation(APawn* Pawn);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
		USceneComponent* ReagentAttachmentPoint;

	UPROPERTY(Replicated)
	AReagentContainer* ReagentContainer;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Ingredients)
		void AddReagent(EReagents Reagent, int32 Amount);

	void AddReagent_Implementation(EReagents Reagent, int32 Amount);

	bool AddReagent_Validate(EReagents Reagent, int32 Amount) { return true; };

	UFUNCTION(BlueprintCallable, Category = Ingredients)
		AReagentContainer* GetReagentContainer();
};
