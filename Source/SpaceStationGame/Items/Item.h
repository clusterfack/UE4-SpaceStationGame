// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SmoothPhysicsState.h"
#include "SlateGameResources.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SWidget.h"
#include "Item.generated.h"

#define PROXY_STATE_ARRAY_SIZE 20

UCLASS()
class SPACESTATIONGAME_API AItem : public AActor
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_ServerPhysicsState)
	FSmoothPhysicsState ServerPhysicsState;

	UFUNCTION()
	void OnRep_ServerPhysicsState();
	

private:
	/** Clients store twenty states with "playback" information from the server. This
	array contains the official state of this object at different times according to
	the server. */
	FSmoothPhysicsState proxyStates[PROXY_STATE_ARRAY_SIZE];

	/** Keep track of what slots are used */
	int proxyStateCount;

	/** Simulates movement **/
	void ClientSimulate();

public:	

	// Enables lag compensation. Only works when physics are being simulated, too
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Replication)
		bool bEnableLagCompensation;

	// Sets default values for this actor's properties
	AItem(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		UStaticMeshComponent* CustomDepthMesh;

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = Default, meta = (DisplayName = "Item: Used"))
		virtual void Use(APawn* Pawn);

	virtual void Use_Implementation(APawn* Pawn);

	virtual bool Use_Validate(APawn* Pawn) { return true; };

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = Default, meta = (DisplayName = "Item: Dropped"))
		virtual void Drop(APawn* Pawn);

	virtual void Drop_Implementation(APawn* Pawn);

	virtual bool Drop_Validate(APawn* Pawn) { return true; };

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = GUI)
		class USlateBrushAsset* GetInventoryIcon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		bool bStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 MaxNumberOfStackItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Item)
		TArray<AItem*> ItemStack;

	// Use the hologram when dropping this item?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		bool bUseHologram;

	// If this is true, then render a client side hologram model
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
		bool bBeingDropped;

	// Only drop this item onto a grid?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		bool bUseGrid;

	// If this item fits onto a grid/tile, whats the size of the grid/tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		FVector GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		FVector PlaceOffset;

protected:

	UPROPERTY(EditDefaultsOnly, Category = Icon)
		class USlateBrushAsset* InventoryIcon;
};