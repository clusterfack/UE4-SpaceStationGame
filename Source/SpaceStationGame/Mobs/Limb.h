// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "Limb.generated.h"

class ULimb;

USTRUCT(BlueprintType)
struct FLimbDamageStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		float Suffocation_Damage;

	UPROPERTY()
		float Burn_Damage;

	UPROPERTY()
		float Toxin_Damage;

	UPROPERTY()
		float Brute_Damage;
};

USTRUCT(BlueprintType)
struct FLimbDamageEvent : public FDamageEvent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		ULimb* Limb;

	UPROPERTY()
		FLimbDamageStruct LimbDamage;
};


UCLASS( ClassGroup=(Limbs), meta=(BlueprintSpawnableComponent) )
class SPACESTATIONGAME_API ULimb : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULimb();

	UPROPERTY(EditAnywhere, Replicated, Category = Health)
		FLimbDamageStruct LimbDamage;

	UFUNCTION(BlueprintCallable, Category = Health)
		FLimbDamageStruct GetLimbDamage();

	// Called when the game starts
	virtual void InitializeComponent() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = Health)
		virtual void AddLimbDamage(FLimbDamageStruct Damage);

	virtual void AddLimbDamage_Implementation(FLimbDamageStruct Damage);

	bool AddLimbDamage_Validate(FLimbDamageStruct Damage) { return true; };
};
