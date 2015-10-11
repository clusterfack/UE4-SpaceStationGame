// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include <map>
#include <unordered_map>
#include "SpaceStationGameCharacter.h"
#include "SpaceStationGameGameState.h"
#include "RecipeContainer.h"
#include "ReagentContainer.h"

#include "UnrealNetwork.h"

AReagentContainer::AReagentContainer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Recipe.OwningContainer = this;
}

void AReagentContainer::AddReagent_Implementation(EReagents Reagent, int32 Amount)
{
	if (Amount < 0)
	{
		Amount = 0;
	}

	auto NewReagent = std::make_pair(Reagent, Amount);

	// use this func for replication
	Recipe.InsertReagentIngredients(NewReagent);

	// if, after we insert the new ingredients, the ingredients turn into a valid recipe, then replace it with its output recipe
	if (GetWorld()->GetGameState<ASpaceStationGameGameState>()->GetRecipeExists(Recipe))
	{
		ASpaceStationGameGameState* GameState = GetWorld()->GetGameState<ASpaceStationGameGameState>();

		Recipe.SetReagentIngredients(GameState->GetProductReagentFromRecipe(Recipe));

		for (auto RecipeIter = Recipe.Ingredients.begin(); RecipeIter != Recipe.Ingredients.end(); RecipeIter++)
		{
			if (RecipeIter->first == EReagents::EChem_Explosion)
			{
				Explode(RecipeIter->second);
			}
		}
	}
}

void AReagentContainer::Explode(uint32 ExplosionSize)
{
	
}

void AReagentContainer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

uint8 AReagentContainer::GetReagentEnumInt(EReagents Reagent)
{
	return (uint8)Reagent; 
}

void AReagentContainer::UseReagentContainerOnChemDispenser_Implementation(FVector NewMeshLocation)
{
	Mesh->RegisterComponent();

	CustomDepthMesh->RegisterComponent();

	Mesh->SetWorldLocation(NewMeshLocation);

	Mesh->PutAllRigidBodiesToSleep();

	UseReagentContainerOnChemDispenser_Client(NewMeshLocation);

	UpdateReagentsOnUse_Implementation();
}

void AReagentContainer::UpdateReagentsOnUse_Implementation()
{
	Recipe.SetReagentIngredients(Recipe);
}