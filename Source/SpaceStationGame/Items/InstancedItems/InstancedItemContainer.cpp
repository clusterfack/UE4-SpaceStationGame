// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "UnrealNetwork.h"
#include "SpaceStationGameCharacter.h"
#include "InstancedItem.h"
#include "InstancedItemContainer.h"


AInstancedItemContainer::AInstancedItemContainer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	InstancedMesh = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Mesh"));
	RootComponent = InstancedMesh;

	InstancedMesh->SetIsReplicated(false);

	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//This actor should ONLY run on the client side, it is a model representation and nothing more
	bReplicates = false;
	bReplicateMovement = false;

	InstancedItemClass = AInstancedItem::StaticClass();

}

void AInstancedItemContainer::SetupInstances()
{
	//TArray<AInstancedItem*> InstancedItems;

	for (TActorIterator<AInstancedItem> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->IsA(InstancedItemClass))
		{
			InstancedMesh->AddInstanceWorldSpace(ActorItr->Mesh->GetComponentTransform());

			InstancedItems.Add(*ActorItr);

			ActorItr->AssociatedContainer = this;

			ActorItr->Mesh->SetVisibility(false);
		}
	}
}

int32 AInstancedItemContainer::AddInstance(const FTransform& WorldTransform, bool SpawnItem)
{
	return InstancedMesh->AddInstanceWorldSpace(WorldTransform);
}

void AInstancedItemContainer::Use(APawn* Pawn, AInstancedItem* ItemCalling)
{
	auto Index = InstancedItems.IndexOfByKey(ItemCalling);

	if (InstancedMesh->RemoveInstance(Index)) InstancedItems.RemoveAt(Index);
}
