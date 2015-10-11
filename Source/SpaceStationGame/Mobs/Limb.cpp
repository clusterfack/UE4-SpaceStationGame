// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "UnrealNetwork.h"
#include "Limb.h"


// Sets default values for this component's properties
ULimb::ULimb()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	SetNetAddressable();
	SetIsReplicated(true);
}

void ULimb::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to only owner
	DOREPLIFETIME_CONDITION(ULimb, LimbDamage, COND_OwnerOnly);
}

// Called when the game starts
void ULimb::InitializeComponent()
{
	Super::InitializeComponent();
}


// Called every frame
void ULimb::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void ULimb::AddLimbDamage_Implementation(FLimbDamageStruct Damage)
{
	LimbDamage.Brute_Damage += Damage.Brute_Damage;
	LimbDamage.Burn_Damage += Damage.Burn_Damage;
	LimbDamage.Suffocation_Damage += Damage.Suffocation_Damage;
	LimbDamage.Toxin_Damage += Damage.Toxin_Damage;

	if (LimbDamage.Brute_Damage <= 0)
	{
		LimbDamage.Brute_Damage = 0;
	}

	if (LimbDamage.Burn_Damage <= 0)
	{
		LimbDamage.Burn_Damage = 0;
	}

	if (LimbDamage.Suffocation_Damage <= 0)
	{
		LimbDamage.Suffocation_Damage = 0;
	}

	if (LimbDamage.Toxin_Damage <= 0)
	{
		LimbDamage.Toxin_Damage = 0;
	}
}

FLimbDamageStruct ULimb::GetLimbDamage()
{
	return LimbDamage;
}