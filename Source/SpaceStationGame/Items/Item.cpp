// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "Item.h"
#include "SpaceStationGameCharacter.h"
#include "SpaceStationGameGameResources.h"
#include "SWidget.h"
#include "UnrealNetwork.h"
#include "SmoothPhysicsState.h"
#include "SpaceStationGamePlayerController.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	RootComponent = Scene;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->AttachTo(Scene);

	CustomDepthMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Custom Depth Mesh"));
	CustomDepthMesh->AttachTo(Mesh);

	CustomDepthMesh->bRenderCustomDepth = false;
	CustomDepthMesh->bRenderInMainPass = false;
	CustomDepthMesh->bCastDynamicShadow = false;
	CustomDepthMesh->bCastStaticShadow = false;
	CustomDepthMesh->bCastHiddenShadow = false;
	CustomDepthMesh->bCastVolumetricTranslucentShadow = false;
	CustomDepthMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/CustomDepthMat.CustomDepthMat'"));

	if (Material.Object != NULL && CustomDepthMesh->IsValidLowLevel())
	{
		for (int32 MatIndexInt = 0; MatIndexInt <= CustomDepthMesh->GetNumMaterials(); MatIndexInt++)
		{
			CustomDepthMesh->SetMaterial(MatIndexInt, (UMaterial*)Material.Object);
		}
	}

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	proxyStateCount = 0;

	bReplicates = true;
	bReplicateMovement = true;

	Mesh->SetIsReplicated(true);
	CustomDepthMesh->SetIsReplicated(true);

	PrimaryActorTick.bCanEverTick = true;

	bStackable = false;

	MaxNumberOfStackItems = 50;
}

void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, ServerPhysicsState);
	DOREPLIFETIME(AItem, bEnableLagCompensation);
	DOREPLIFETIME_CONDITION(AItem, ItemStack, COND_OwnerOnly);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (Role < ROLE_Authority)
	{
		UPrimitiveComponent *Root = Cast<UPrimitiveComponent>(GetRootComponent());

		Mesh->PutRigidBodyToSleep();
		if (Mesh)
		{
			Mesh->SetSimulatePhysics(false);
		}

		Mesh->SetEnableGravity(false);

		SetActorEnableCollision(true);
	}
}

void AItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if (Mesh && Mesh->IsSimulatingPhysics() && bEnableLagCompensation)
	{
		if (Role < ROLE_Authority)
		{
			// Clients should update its local position based on where it is on the server
			ClientSimulate();
		}
		else
		{
			// Servers should simulate physics freely and replicate the orientation
			UPrimitiveComponent *Root = Cast<UPrimitiveComponent>(GetRootComponent());
			ServerPhysicsState.pos = GetActorLocation();
			ServerPhysicsState.rot = GetActorRotation();
			ServerPhysicsState.vel = Root->GetComponentVelocity();
			ServerPhysicsState.timestamp = ASpaceStationGamePlayerController::GetLocalTime();
		}
	}
}

void AItem::Use_Implementation(APawn* Pawn)
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
	}
	else if (bStackable)
	{
		AItem* Item = Cast<ASpaceStationGameCharacter>(Pawn)->GetInventoryItem(Cast<ASpaceStationGameCharacter>(Pawn)->GetSelectedItem());

		if (IsA(Item->GetClass()))
		{
			if ((Item->ItemStack.Num() + 1) < Item->MaxNumberOfStackItems)
			{
				if (GetWorld()->IsServer())
				{
					Item->ItemStack.Add(this);
					SetOwner(Pawn);
				}

				if (Mesh)
				{
					Mesh->UnregisterComponent(); // physical item has been picked up, destroy its visible component
				}

				if (CustomDepthMesh)
				{
					CustomDepthMesh->UnregisterComponent();
				}
			}
		}
	}
}

void AItem::Drop_Implementation(APawn* Pawn)
{
	if (Pawn->IsValidLowLevel() && Cast<ASpaceStationGameCharacter>(Pawn))
	{
		auto SpaceStationGameChar = Cast<ASpaceStationGameCharacter>(Pawn);

		if (bStackable && ItemStack.Num() > 0)
		{
			if (ItemStack.Last()->Mesh)
			{
				ItemStack.Last()->Mesh->RegisterComponent();
			}

			if (ItemStack.Last()->CustomDepthMesh)
			{
				ItemStack.Last()->CustomDepthMesh->RegisterComponent();
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

					ItemStack.Last()->RootComponent->SetWorldLocation(Hit.Location);
				}
				else
				{
					if (ItemStack.Last()->bUseGrid && ItemStack.Last()->GridSize.X > 0 && ItemStack.Last()->GridSize.Y > 0 && ItemStack.Last()->GridSize.Z > 0)
					{
						FVector NewLocation = EndTrace;

						NewLocation /= ItemStack.Last()->GridSize;

						int32 X = NewLocation.X;
						int32 Y = NewLocation.Y;
						int32 Z = NewLocation.Z;

						NewLocation = FVector(X * ItemStack.Last()->GridSize.X, Y * ItemStack.Last()->GridSize.Y, Z * ItemStack.Last()->GridSize.Z);

						NewLocation += PlaceOffset;

						ItemStack.Last()->RootComponent->SetWorldLocation(NewLocation);
					}
					else
					{
						auto NewEndTrace = EndTrace + PlaceOffset;

						ItemStack.Last()->RootComponent->SetWorldLocation(NewEndTrace);
					}
				}
			}

			ItemStack.RemoveAt(ItemStack.Num() - 1);
		}
		else
		{
			if (Mesh)
			{
				Mesh->RegisterComponent();
			}

			if (CustomDepthMesh)
			{
				CustomDepthMesh->RegisterComponent();
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
					RootComponent->SetWorldLocation(Hit.Location);
				}
				else
				{
					RootComponent->SetWorldLocation(EndTrace);
				}
			}

			if (GetWorld()->IsServer())
			{
				SpaceStationGameChar->RemoveInventoryItem(SpaceStationGameChar->GetSelectedItem());
			}
		}
	}
}

class USlateBrushAsset* AItem::GetInventoryIcon()
{
	return InventoryIcon;
}

// Stupid physics simulation thingy, might come back to this at a later date


void AItem::OnRep_ServerPhysicsState()
{

	// If we get here, we are always the client. Here we store the physics state
	// for physics state interpolation.
	// Shift the buffer sideways, deleting state PROXY_STATE_ARRAY_SIZE

	for (int i = PROXY_STATE_ARRAY_SIZE - 1; i >= 1; i--)
	{
		proxyStates[i] = proxyStates[i - 1];
	}

	// Record current state in slot 0

	proxyStates[0] = ServerPhysicsState;

	// Update used slot count, however never exceed the buffer size
	// Slots aren't actually freed so this just makes sure the buffer is
	// filled up and that uninitalized slots aren't used.

	proxyStateCount = FMath::Min(proxyStateCount + 1, PROXY_STATE_ARRAY_SIZE);

	// Check if states are in order

	if (proxyStates[0].timestamp < proxyStates[1].timestamp)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Timestamp inconsistent: %d should be greater than %d"), proxyStates[0].timestamp, proxyStates[1].timestamp);
	}
}

void AItem::ClientSimulate()
{
	ASpaceStationGamePlayerController* MyPC = Cast<ASpaceStationGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (nullptr == MyPC || !MyPC->IsNetworkTimeValid() || 0 == proxyStateCount)
	{
		// We don't know yet know what the time is on the server yet so the timestamps

		// of the proxy states mean nothing; that or we simply don't have any proxy

		// states yet. Don't do any interpolation.

		SetActorLocationAndRotation(ServerPhysicsState.pos, ServerPhysicsState.rot);
	}
	else
	{
		uint64 interpolationBackTime = 100;

		uint64 extrapolationLimit = 500;

		// This is the target playback time of the rigid body

		uint64 interpolationTime = MyPC->GetNetworkTime() - interpolationBackTime;

		// Use interpolation if the target playback time is present in the buffer

		if (proxyStates[0].timestamp > interpolationTime)
		{
			// Go through buffer and find correct state to play back

			for (int i = 0; i < proxyStateCount; i++)
			{
				if (proxyStates[i].timestamp <= interpolationTime || i == proxyStateCount - 1)
				{

					// The state one slot newer (<100ms) than the best playback state

					FSmoothPhysicsState rhs = proxyStates[FMath::Max(i - 1, 0)];

					// The best playback state (closest to 100 ms old (default time))

					FSmoothPhysicsState lhs = proxyStates[i];

					// Use the time between the two slots to determine if interpolation is necessary

					int64 length = (int64)(rhs.timestamp - lhs.timestamp);

					double t = 0.0F;

					// As the time difference gets closer to 100 ms t gets closer to 1 in

					// which case rhs is only used

					if (length > 1)

						t = (double)(interpolationTime - lhs.timestamp) / (double)length;

					// if t=0 => lhs is used directly

					FVector pos = FMath::Lerp(lhs.pos, rhs.pos, t);

					FRotator rot = FMath::Lerp(lhs.rot, rhs.rot, t);

					SetActorLocationAndRotation(pos, rot);

					return;
				}
			}
		}
		// Use extrapolation
		else
		{

			FSmoothPhysicsState latest = proxyStates[0];

			uint64 extrapolationLength = interpolationTime - latest.timestamp;

			// Don't extrapolate for more than [extrapolationLimit] milliseconds

			if (extrapolationLength < extrapolationLimit)

			{

				FVector pos = latest.pos + latest.vel * ((float)extrapolationLength * 0.001f);

				FRotator rot = latest.rot;

				SetActorLocationAndRotation(pos, rot);

			}

			else

			{

				// Don't move. If we're this far away from the server, we must be pretty laggy.

				// Wait to catch up with the server.

			}

		}

	}

}
