// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PECharacter.h"
#include "Actors/Character/PEPlayerController.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GameFrameworkComponentManager.h"

DEFINE_LOG_CATEGORY(LogPlayerState);

APEPlayerState::APEPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UPEAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Data.Game.Player"));

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetUpdateFrequency = 100.f;
}

void APEPlayerState::BeginPlay()
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called."), *FString(__func__));

	Super::BeginPlay();

	if (ensureMsgf(IsValid(AbilitySystemComponent), TEXT("%s have a invalid AbilitySystemComponent"), *GetName()))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Dead")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(this,
			&APEPlayerState::DeathStateChanged_Callback);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Stunned")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(this,
			&APEPlayerState::StunStateChanged_Callback);
	}
}

APEPlayerController* APEPlayerState::GetPEPlayerController() const
{
	return Cast<APEPlayerController>(GetPlayerController());
}

void APEPlayerState::DeathStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	if (!HasAuthority())
	{
		return;
	}

	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
	                 *FString(__func__),
	                 *CallbackTag.ToString(), NewCount);

	if (NewCount != 0)
	{
		if (APEPlayerController* Controller_Temp = GetPEPlayerController();
			ensureMsgf(IsValid(Controller_Temp), TEXT("%s have a invalid Controller"), *GetName()))
		{
			if (APECharacter* Player_Temp = Controller_Temp->GetPawn<APECharacter>();
				ensureMsgf(IsValid(Player_Temp), TEXT("%s have a invalid Player"), *GetName()))
			{
				Player_Temp->PerformDeath();
			}

			Controller_Temp->SetupControllerSpectator();
		}
	}
}

void APEPlayerState::StunStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	if (!HasAuthority())
	{
		return;
	}

	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
	                 *FString(__func__),
	                 *CallbackTag.ToString(), NewCount);

	if (ensureMsgf(IsValid(GetPlayerController()), TEXT("%s have a invalid Player"), *GetName()))
	{
		GetPlayerController()->SetIgnoreMoveInput(NewCount != 0);
	}
}

UAbilitySystemComponent* APEPlayerState::GetAbilitySystemComponent() const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called"), *FString(__func__));
	return AbilitySystemComponent;
}
