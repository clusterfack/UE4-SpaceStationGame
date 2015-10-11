// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "InstancedItem.generated.h"

class AInstancedItemContainer;

UCLASS(Meta=(ChildCanTick))
class SPACESTATIONGAME_API AInstancedItem : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInstancedItem(const FObjectInitializer& ObjectInitializer);

	virtual void Use_Implementation(APawn* Pawn) override;

	virtual void Drop_Implementation(APawn* Pawn) override;

	UFUNCTION(Client, Unreliable)
		void ClientSpawnInstanceEffects(APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent, Category = Effects)
		void ClientSpawnInstanceEffects_Implementation(APawn* Pawn);

	UFUNCTION(NetMulticast, Reliable)
		void AddInstance(FVector Location);

	void AddInstance_Implementation(FVector Location);

	AInstancedItemContainer* AssociatedContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		TSubclassOf<class AInstancedItemContainer> InstancedItemContainerClass;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};