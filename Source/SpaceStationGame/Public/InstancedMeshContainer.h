// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "InstancedMeshContainer.generated.h"

UCLASS()
class SPACESTATIONGAME_API AInstancedMeshContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInstancedMeshContainer(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		UInstancedStaticMeshComponent* InstancedMesh;

	//TArray<AInstancedItem*> InstancedItems;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual int32 AddInstance(const FTransform& WorldTransform, bool SpawnItem);

	virtual void Use(APawn* Pawn);
};
