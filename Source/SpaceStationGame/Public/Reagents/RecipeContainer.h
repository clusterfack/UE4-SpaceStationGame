// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Map.h"
#include <map>
#include <unordered_map>
#include "Reagents.h"
//#include "ReagentContainer.h"

class AReagentContainer;

#include "RecipeContainer.generated.h"

#define RECIPEFUNCS_MAX 1024

USTRUCT()
struct FRecipe
{
	GENERATED_USTRUCT_BODY()

	std::map<EReagents, uint32> Ingredients;

	AReagentContainer* OwningContainer;

	bool operator<(const FRecipe& rhs) const
	{
		if (Ingredients.size() < rhs.Ingredients.size()) return true;
		if (Ingredients.size() > rhs.Ingredients.size()) return false;
		for (const auto& part : Ingredients)
		{
			if (!rhs.Ingredients.count(part.first)) return false;
			if (rhs.Ingredients.at(part.first) < part.second) return false;
			if (rhs.Ingredients.at(part.first) > part.second) return true;
		}
		return false;
	}

	void InsertReagentIngredients(std::pair<EReagents, uint32> IngredientsPair);

	void SetReagentIngredients(FRecipe InRecipe);
};

UCLASS()
class SPACESTATIONGAME_API URecipeContainer : public UObject
{
	GENERATED_BODY()

public:
	std::map<FRecipe, FRecipe> RecipeMap;

	void ConstructRecipe(std::map<EReagents, uint32> Ingredients, std::map<EReagents, uint32> Products);

	std::map<FRecipe, FRecipe> GetRecipeMap() { return RecipeMap; };
};