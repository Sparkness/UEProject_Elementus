// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "PECharacterBase.generated.h"

class UGameplayAbility;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FGameplayTag;
/**
 *
 */
UCLASS(config = Game, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APECharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

protected:
	virtual void PossessedBy(AController* InputController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

private:
	void InitializeABSC(const bool bOnRep);
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	bool bIsFrameworkReady = false;

public:
	APECharacterBase(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Character", GetFName());
	}

	/** Returns CameraBoom sub object **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}

	/** Returns FollowCamera sub object **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	/** Returns FollowCamera Forward Vector **/
	const FVector GetCameraForwardVector() const;

	/** Returns FollowCamera Location at World **/
	const FVector GetCameraComponentLocation() const;

	/** Returns FollowCamera Location at World **/
	const float GetCameraTargetArmLength() const;

	/* Returns character default walk speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultWalkSpeed() const;

	/* Returns character default crouch speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultCrouchSpeed() const;

	/* Returns character default jump velocity */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultJumpVelocity() const;

	/* Returns character associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* Enumeration class used to bind ability InputIDs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "InputID Enumeration Class"),
		Category = "Custom GAS | Data")
		UEnum* InputIDEnumerationClass;

protected:
	float DefaultWalkSpeed, DefaultCrouchSpeed, DefaultJumpVelocity;

	/* Array of given abilities */
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Abilities")
		TArray<TSubclassOf<UGameplayAbility>> CharacterAbilities;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/* Give a new Ability to the Player -  bAutoAdjustInput will ignore InputId and select Skill_1, Skill_2 or Skill_3 based on current owned abilities */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
		void GiveAbility(TSubclassOf<UGameplayAbility> Ability, const FName InputId,
			const bool bTryRemoveExistingAbilityWithInput, const bool bTryRemoveExistingAbilityWithClass);
	virtual void GiveAbility_Implementation(TSubclassOf<UGameplayAbility> Ability, const FName InputId,
		const bool bTryRemoveExistingAbilityWithInput,
		const bool bTryRemoveExistingAbilityWithClass);

	/* Will remove the ability associated to the InputAction */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
		void RemoveAbility(TSubclassOf<UGameplayAbility> Ability);
	virtual void RemoveAbility_Implementation(TSubclassOf<UGameplayAbility> Ability);

	/* Init a death state with this character */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Custom GAS | Behaviors")
		void PerformDeath();
	virtual void PerformDeath_Implementation();
	bool PerformDeath_Validate();

private:
	virtual void Landed(const FHitResult& Hit) final override;
};
