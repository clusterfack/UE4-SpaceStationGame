// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/InstancedItems/InstancedItem.h"
#include "LatticeInstancedItem.generated.h"

/**
 * 
 */
UCLASS()
class SPACESTATIONGAME_API ALatticeInstancedItem : public AInstancedItem
{
	GENERATED_BODY()

public:
	ALatticeInstancedItem(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics)
		bool bEnableGravity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		UBoxComponent* Box;
	
	UFUNCTION()
		void OnBoxOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnBoxOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
