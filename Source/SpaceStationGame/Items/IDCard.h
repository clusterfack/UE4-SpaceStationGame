// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Jobs.h"
#include "IDCard.generated.h"

#define PROXY_STATE_ARRAY_SIZE 20

UCLASS()
class SPACESTATIONGAME_API AIDCard : public AItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = ID)
		FString AssignedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = ID)
		EJobs AssignedJob;

public:
	// Sets default values for this actor's properties
	AIDCard(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = ID, meta = (DisplayName = "Get ID Name"))
		FString GetAssignedName();

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = ID, meta = (DisplayName = "Set ID Name"))
		void ServerSetAssignedName(const FString& NewName);

	void ServerSetAssignedName_Implementation(const FString& NewName);

	bool ServerSetAssignedName_Validate(const FString& NewName) { return true; };

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = ID, meta = (DisplayName = "Set Job"))
		void ServerSetJob(EJobs Job);

	void ServerSetJob_Implementation(EJobs Job);

	bool ServerSetJob_Validate(EJobs Job) { return true; };
};