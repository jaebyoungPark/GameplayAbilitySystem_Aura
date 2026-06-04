// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"

#include "DebugHelper.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	/*AB_LOG(LogTemp, Warning, TEXT("InCooldownTag : %s"), *InCooldownTag.ToString());*/

	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	// To Know when a cooldown has ended (Cooldown tag has been removed)
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCooldownChange,
			&UWaitCooldownChange::CooldownTagChanged);

	// To Know when a cooldown effect has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);


	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	/*AB_LOG(LogTemp, Warning, TEXT("NewCount : %d"), NewCount);*/
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}

}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	/*AB_LOG(LogTemp, Warning, TEXT("this : %s, EffectSpec's Def : %s"), *GetFName().ToString(), *SpecApplied.Def->GetName());*/

	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	/*AB_LOG(LogTemp, Warning, TEXT("[AssetTags] : %s"), *AssetTags.ToString());*/

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	AB_LOG(LogTemp, Warning, TEXT("[GrantedTags] : %s"), *GrantedTags.ToString());

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

		for (auto TimeRemaining : TimesRemaining)
		{
			AB_LOG(LogTemp, Warning, TEXT("[TimeRemaining] : %.2f"), TimeRemaining);
		}

		if (TimesRemaining.Num() > 0)
		{
			float TimeRemaining = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > TimeRemaining)
				{
					TimeRemaining = TimesRemaining[i];
					
				}

			}

			CooldownStart.Broadcast(TimeRemaining);
		}

	}

}
