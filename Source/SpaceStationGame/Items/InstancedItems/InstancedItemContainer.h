// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "InstancedItem.h"
#include "InstancedItemContainer.generated.h"

/*USTRUCT()
struct FItemContainerStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		AInstancedItemContainer* AssociatedClass;
};*/

UCLASS()
class SPACESTATIONGAME_API AInstancedItemContainer : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AInstancedItemContainer(const FObjectInitializer& ObjectInitializer);
	
	TArray<AInstancedItem*> InstancedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		TSubclassOf<class AInstancedItem> InstancedItemClass;

	void SetupInstances();

	virtual int32 AddInstance(const FTransform& WorldTransform, bool SpawnItem);

	virtual void Use(APawn* Pawn, AInstancedItem* ItemCalling);
};