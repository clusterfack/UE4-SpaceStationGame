// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "SpaceStationGameGameState.h"
#include "SpaceStationGameCharacter.h"
#include "InstancedItemContainer.h"
#include "InstancedItem.h"


// Sets default values
AInstancedItem::AInstancedItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh->SetIsReplicated(true);

	CustomDepthMesh->AttachTo(Scene);

	bReplicates = true;
	bReplicateMovement = true;

	InstancedItemContainerClass = AInstancedItemContainer::StaticClass();
}

// Called when the game starts or when spawned
void AInstancedItem::BeginPlay()
{

}

void AInstancedItem::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );
}

void AInstancedItem::Use_Implementation(APawn* Pawn)
{
	if (!Cast<ASpaceStationGameCharacter>(Pawn)->InventoryItemIsValid(Cast<ASpaceStationGameCharacter>(Pawn)->GetSelectedItem()))
	{
		if (Mesh)
		{
			Mesh->UnregisterComponent(); // physical item has been picked up, destroy its visible component
		}

		if (CustomDepthMesh)
		{
			CustomDepthMesh->UnregisterComponent();
		}

		if (GetWorld()->IsServer())
		{
			SetOwner(Pawn);
			Cast<ASpaceStationGameCharacter>(Pawn)->AddInventoryItemIndex(this, Cast<ASpaceStationGameCharacter>(Pawn)->GetSelectedItem());
		}

		if (AssociatedContainer->IsValidLowLevel())
		{
			AssociatedContainer->Use(Pawn, this);
		}

		if (!HasAuthority())
		{
			ClientSpawnInstanceEffects(Pawn);
		}
	}
	else if (bStackable)
	{
		AItem* Item = Cast<ASpaceStationGameCharacter>(Pawn)->GetInventoryItem(Cast<ASpaceStationGameCharacter>(Pawn)->GetSelectedItem());

		if (IsA(Item->GetClass()))
		{
			if ((Item->ItemStack.Num() + 1) < Item->MaxNumberOfStackItems)
			{
				Item->ItemStack.Add(this);

				if (Mesh)
				{
					Mesh->UnregisterComponent(); // physical item has been picked up, destroy its visible component
				}

				if (CustomDepthMesh)
				{
					CustomDepthMesh->UnregisterComponent();
				}

				if (GetWorld()->IsServer())
				{
					SetOwner(Pawn);
				}

				if (AssociatedContainer->IsValidLowLevel())
				{
					AssociatedContainer->Use(Pawn, this);
				}

				if (!HasAuthority())
				{
					ClientSpawnInstanceEffects(Pawn);
				}
			}
		}
	}
}

void AInstancedItem::Drop_Implementation(APawn* Pawn)
{
	if (Pawn->IsValidLowLevel() && Cast<ASpaceStationGameCharacter>(Pawn))
	{
		auto SpaceStationGameChar = Cast<ASpaceStationGameCharacter>(Pawn);

		// if stackable
		if (bStackable && ItemStack.Num() > 0)
		{
			if (ItemStack.Last()->Mesh)
			{
				ItemStack.Last()->Mesh->RegisterComponent();
				ItemStack.Last()->Mesh->SetVisibility(false);
			}

			if (ItemStack.Last()->CustomDepthMesh)
			{
				ItemStack.Last()->CustomDepthMesh->RegisterComponent();
				CustomDepthMesh->SetRenderCustomDepth(false);
			}

			if (HasAuthority())
			{
				FVector CamLoc;
				FRotator CamRot;

				Pawn->GetController()->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
				const FVector StartTrace = CamLoc; // trace start is the camera location
				const FVector Direction = CamRot.Vector();
				const FVector EndTrace = StartTrace + Direction * Cast<ASpaceStationGameCharacter>(Pawn)->MaxDropDistance; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

				// Perform trace to retrieve hit info
				FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
				TraceParams.bTraceAsyncScene = true;
				TraceParams.bReturnPhysicalMaterial = true;
				TraceParams.AddIgnoredActor(Pawn);

				FHitResult Hit(ForceInit);
				if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams))
				{
					if (ItemStack.Last()->bUseGrid && ItemStack.Last()->GridSize.X > 0 && ItemStack.Last()->GridSize.Y > 0 && ItemStack.Last()->GridSize.Z > 0)
					{
						Hit.Location /= ItemStack.Last()->GridSize;

						int32 X = Hit.Location.X;
						int32 Y = Hit.Location.Y;
						int32 Z = Hit.Location.Z;

						Hit.Location = FVector(X * ItemStack.Last()->GridSize.X, Y * ItemStack.Last()->GridSize.Y, Z * ItemStack.Last()->GridSize.Z);
					}

					Hit.Location += PlaceOffset;

					ItemStack.Last()->Scene->SetWorldLocation(Hit.Location);

					AddInstance(Hit.Location);
				}
				else // if the trace works
				{
					if (ItemStack.Last()->bUseGrid && ItemStack.Last()->GridSize.X > 0 && ItemStack.Last()->GridSize.Y > 0 && ItemStack.Last()->GridSize.Z > 0)
					{ //if the item uses the grid
						FVector NewLocation = EndTrace;

						NewLocation /= ItemStack.Last()->GridSize;

						int32 X = NewLocation.X;
						int32 Y = NewLocation.Y;
						int32 Z = NewLocation.Z;

						NewLocation = FVector(X * ItemStack.Last()->GridSize.X, Y * ItemStack.Last()->GridSize.Y, Z * ItemStack.Last()->GridSize.Z);

						NewLocation += PlaceOffset;

						ItemStack.Last()->Scene->SetWorldLocation(NewLocation);

						AddInstance(NewLocation);
					}
					else
					{
						auto NewEndTrace = EndTrace + PlaceOffset;

						ItemStack.Last()->Scene->SetWorldLocation(NewEndTrace);

						AddInstance(NewEndTrace);
					}
				}
			}

			ItemStack.RemoveAt(ItemStack.Num() - 1);
		}
		else // if not stackable
		{
			if (Mesh)
			{
				Mesh->RegisterComponent();
				Mesh->SetVisibility(false);
			}

			if (CustomDepthMesh)
			{
				CustomDepthMesh->RegisterComponent();
				CustomDepthMesh->SetRenderCustomDepth(false);
			}

			if (HasAuthority())
			{
				FVector CamLoc;
				FRotator CamRot;

				Pawn->GetController()->GetPlayerViewPoint(CamLoc, CamRot); //GetController()->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
				const FVector StartTrace = CamLoc; // trace start is the camera location
				const FVector Direction = CamRot.Vector();
				const FVector EndTrace = StartTrace + Direction * Cast<ASpaceStationGameCharacter>(Pawn)->MaxDropDistance; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

				// Perform trace to retrieve hit info
				FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
				TraceParams.bTraceAsyncScene = true;
				TraceParams.bReturnPhysicalMaterial = true;
				TraceParams.AddIgnoredActor(Pawn);

				FHitResult Hit(ForceInit);
				if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams))
				{
					if (bUseGrid && GridSize.X > 0 && GridSize.Y > 0 && GridSize.Z > 0)
					{
						Hit.Location /= GridSize;

						int32 X = Hit.Location.X;
						int32 Y = Hit.Location.Y;
						int32 Z = Hit.Location.Z;

						Hit.Location = FVector(X * GridSize.X, Y * GridSize.Y, Z * GridSize.Z);
					}

					Hit.Location += PlaceOffset;

					RootComponent->SetWorldLocation(Hit.Location);

					AddInstance(Hit.Location);
				}
				else
				{
					if (bUseGrid && GridSize.X > 0 && GridSize.Y > 0 && GridSize.Z > 0)
					{
						FVector NewLocation = EndTrace;

						NewLocation /= GridSize;

						int32 X = NewLocation.X;
						int32 Y = NewLocation.Y;
						int32 Z = NewLocation.Z;

						NewLocation = FVector(X * GridSize.X, Y * GridSize.Y, Z * GridSize.Z);

						NewLocation += PlaceOffset;
						
						RootComponent->SetWorldLocation(NewLocation);

						AddInstance(NewLocation);
					}
					else
					{
						auto NewEndTrace = EndTrace + PlaceOffset;

						RootComponent->SetWorldLocation(NewEndTrace);

						AddInstance(NewEndTrace);
					}
				}
			}

			if (GetWorld()->IsServer())
			{
				SpaceStationGameChar->RemoveInventoryItem(SpaceStationGameChar->GetSelectedItem());
			}
		}
	}
}

void AInstancedItem::AddInstance_Implementation(FVector Location)
{
	if (AssociatedContainer->IsValidLowLevel())
	{
		FTransform Transform(FRotator(0, 0, 0), Location, FVector(1, 1, 1));

		AssociatedContainer->AddInstance(Transform, false);

		AssociatedContainer->InstancedItems.Add(this);
	}
	else if (!HasAuthority())
	{
		auto GameState = Cast<ASpaceStationGameGameState>(GetWorld()->GetGameState());

		for (auto Itr = GameState->InstancedItemContainers.CreateIterator(); Itr; ++Itr)
		{
			auto PtrItr = *Itr;

			if (Cast<UClass>(InstancedItemContainerClass) == PtrItr->GetClass())
			{
				AssociatedContainer = PtrItr;

				AssociatedContainer->AddInstance(Scene->GetComponentTransform(), false);

				return ItemStack.RemoveAt(ItemStack.Num() - 1);
			}
		}
	}
}