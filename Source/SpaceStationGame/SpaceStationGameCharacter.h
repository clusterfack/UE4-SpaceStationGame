// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Map.h"

#include "Item.h"

#include "BloodTypeStruct.h"
#include "Limb.h"
#include "Inventory.h"
#include "Reagents.h"
#include "SpaceStationGameCharacter.generated.h"

class UInputComponent;

UENUM()
namespace EHealthCalculationMethod
{
	enum Type
	{
		Additive UMETA(DisplayName = "Additive health", ToolTip = "Damage from limbs is just added on to the general health variable. Least realistic but most akin to classic SS13 health."),
		Realistic UMETA(DisplayName = "Realistic health" ToolTip = "TBA"),
	};
}

UCLASS(config=Game)
class ASpaceStationGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated)
		FInventory InventoryStruct;

	//TMap<EHumanInventorySlot, AItem*> InventoryMap;

	UPROPERTY()
		ULimb* LeftArm;

	UPROPERTY()
		ULimb* RightArm;

	void CalculateHealth(float Damage, struct FDamageEvent const& DamageEvent);

	UPROPERTY(Replicated)
		AItem* AffectedItem;

public:
	ASpaceStationGameCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = Use)
		float MaxUseDistance;

	UPROPERTY(EditAnywhere, Category = Use)
		float MaxDropDistance;

	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, Replicated, Category = Health)
		float Health;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetHealth();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Health)
		void SetHealth(float NewHealth);

	bool SetHealth_Validate(float NewHealth) {	return true; };

	void SetHealth_Implementation(float NewHealth);

	UPROPERTY(EditAnyWhere, Replicated, Category = Inventory)
		EHumanInventorySlot SelectedItem;

	UPROPERTY(EditAnywhere, Replicated, Category = Health)
		float BloodVolume;

	UPROPERTY(EditAnywhere, Replicated, Category = Health)
		FBloodTypeStruct BloodType;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ASpaceStationGameProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool InventoryItemIsValid(EHumanInventorySlot Index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AItem* GetInventoryItem(EHumanInventorySlot Index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		uint8 GetInventoryItemByValue(AItem* Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void SwapInventoryItem(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2);

	// Make a client and a server function
	UFUNCTION(Server, WithValidation, Reliable)
		void SwapInventoryItem_Server(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2);

		bool SwapInventoryItem_Server_Validate(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2) { return true; };

		void SwapInventoryItem_Server_Implementation(EHumanInventorySlot Slot1, EHumanInventorySlot Slot2);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = Inventory)
		void RemoveInventoryItem(EHumanInventorySlot Index);

	bool RemoveInventoryItem_Validate(EHumanInventorySlot Index) { return true; };

	void RemoveInventoryItem_Implementation(EHumanInventorySlot Index);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = Inventory)
		void AddInventoryItemIndex(AItem* Item, EHumanInventorySlot Index);

	bool AddInventoryItemIndex_Validate(AItem* Item, EHumanInventorySlot Index) { return true; };

	void AddInventoryItemIndex_Implementation(AItem* Item, EHumanInventorySlot Index);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Unreliable, Category = Inventory)
		void SwapSelectedItem();

	bool SwapSelectedItem_Validate() { return true; };

	void SwapSelectedItem_Implementation();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		EHumanInventorySlot GetSelectedItem();
	
	void Enter();

	UFUNCTION(BlueprintCallable, Category = Name)
		FString GetPawnName();

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = Name)
		void SetPawnName(const FString& NewName);
	
	bool SetPawnName_Validate(const FString& NewName) { return true; };

	void SetPawnName_Implementation(const FString& NewName);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = Item)
		void SetAffectedItem(AItem* Item);

	bool SetAffectedItem_Validate(AItem* Item) { return true; };

	void SetAffectedItem_Implementation(AItem* Item);

	UFUNCTION(BlueprintCallable, Category = Item)
		AItem* GetAffectedItem();

	UPROPERTY(config, EditAnywhere, Category = Health, meta = (
		ConsoleVariable = "r.Character.HealthCalculationMethod", DisplayName = "Health Calculation Method",
		ToolTip = "How health is calculated. Different methods use more or less realistic calculation methods."))
		TEnumAsByte<EHealthCalculationMethod::Type> HealthCalculationMethod;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Ingredients)
		void AddReagent(EReagents Reagent, int32 Amount);

	void AddReagent_Implementation(EReagents Reagent, int32 Amount);

	bool AddReagent_Validate(EReagents Reagent, int32 Amount) { return true; };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Networking)
		FString OwnedPlayerID;

protected:
	UPROPERTY(Replicated, EditAnywhere, Category = Name)
		FString PawnName;

	/** Fires a projectile. */
	void OnFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void Use();

	virtual void Use_Implementation();

	virtual bool Use_Validate() { return true; };

	virtual void Drop();

	UFUNCTION(Server, Reliable, WithValidation)
		void Drop_Server();

	virtual void Drop_Server_Implementation();

	virtual bool Drop_Server_Validate() { return true; };

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
