// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Engine/DataTable.h"
#include "AbilitySystem/Data/AbilityInfo.h"

#include "DebugHelper.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	
}


void UOverlayWidgetController::BindCallbacksToDependencies()
{
	
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue); 
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilitiesGiven)
		{
			

			//이건 SetWidgetController 를 거쳐 AddCharacterAbilities 까지 왔다는 가정 하에 수행되는듯. 그 이유가 아니라면 SetWidgetController 를 하지 않아 SpellGlobe 에서 AbilityInfoDelegate 가 바인딩
			//되지 않았을 것이고, OnInitializeStartupAbilities 의 ForEachAbilty 내부에서 AbilityInfoDelegate 를 broadcast 하는데, 바인딩 이전이라 문제가 생길수 밖에 없음.
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
		
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags) 
			{
				

				for (const FGameplayTag& Tag : AssetTags)
				{
					//For example, say that Tag = Message.HealthPotion
					//* "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False

					

					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}



}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	

	// TODO : Get information about all given abilities, look up their Ability Info, and broadcast it ot widgets.
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		//FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		//Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);

		//Test
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
		//
	}
	);
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);

}



