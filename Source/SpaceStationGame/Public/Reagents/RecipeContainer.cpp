// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "Reagents.h"
#include "Map.h"
#include <map>
#include <unordered_map>
#include "ReagentContainer.h"
#include "RecipeContainer.h"

void URecipeContainer::ConstructRecipe(std::map<EReagents, uint32> Ingredients, std::map<EReagents, uint32> Products)
{
	FRecipe ReagentsRecipe;
	ReagentsRecipe.Ingredients = Ingredients;

	FRecipe ProductsRecipe;
	ProductsRecipe.Ingredients = Products;

	RecipeMap.insert(std::make_pair(ReagentsRecipe, ProductsRecipe));
}

void FRecipe::InsertReagentIngredients(std::pair<EReagents, uint32> IngredientsPair)
{
	if (Ingredients.count(IngredientsPair.first) > 0)
	{
		Ingredients[IngredientsPair.first] = IngredientsPair.second + Ingredients[IngredientsPair.first];
	}
	else
	{
		Ingredients.insert(IngredientsPair);
	}

	if (OwningContainer)
	{
		if (Ingredients.count(IngredientsPair.first) > 0)
		{
			OwningContainer->UIUpdateAmount(IngredientsPair.first, Ingredients[IngredientsPair.first]);
		}
		else
		{
			OwningContainer->UIUpdateAmount(IngredientsPair.first, IngredientsPair.second);
		}
	}
}

void FRecipe::SetReagentIngredients(FRecipe InRecipe)
{
	Ingredients = InRecipe.Ingredients;

	if (OwningContainer)
	{
		for (auto Iter = Ingredients.begin(); Iter != Ingredients.end(); Iter++)
		{
			OwningContainer->UIUpdateAmount(Iter->first, Iter->second);
		}
	}
}
