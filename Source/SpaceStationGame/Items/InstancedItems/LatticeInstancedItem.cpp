// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "SpaceStationGameCharacter.h"
#include "LatticeInstancedItem.h"

ALatticeInstancedItem::ALatticeInstancedItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Box = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Box"));
	Box->AttachParent = RootComponent;

	Box->OnComponentBeginOverlap.AddDynamic(this, &ALatticeInstancedItem::OnBoxOverlapBegin);
}

void ALatticeInstancedItem::OnBoxOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OtherComp->SetEnableGravity(false);
}

void ALatticeInstancedItem::OnBoxOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<AActor*> OverlappingActors;

	OtherComp->GetOverlappingActors(OverlappingActors, this->StaticClass());

	if (OverlappingActors.Num() == 0)
	{
		OtherComp->SetEnableGravity(true);
	}
}