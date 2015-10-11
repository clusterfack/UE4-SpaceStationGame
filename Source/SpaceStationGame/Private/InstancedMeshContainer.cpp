// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "InstancedMeshContainer.h"


// Sets default values
AInstancedMeshContainer::AInstancedMeshContainer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InstancedMesh = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Mesh"));
	RootComponent = InstancedMesh;
}

// Called when the game starts or when spawned
void AInstancedMeshContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInstancedMeshContainer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

int32 AInstancedMeshContainer::AddInstance(const FTransform& WorldTransform, bool SpawnItem)
{
	return InstancedMesh->AddInstanceWorldSpace(WorldTransform);
}

void AInstancedMeshContainer::Use(APawn* Pawn)
{
	//auto Index = InstancedItems.IndexOfByKey(ItemCalling);

	//if (InstancedMesh->RemoveInstance(Index)) InstancedItems.RemoveAt(Index);
}