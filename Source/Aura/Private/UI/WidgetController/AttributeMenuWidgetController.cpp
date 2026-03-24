// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

		//FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
		//Info.AttributeValue = AS->GetStrength();
		//AttributeInfoDelegate.Broadcast(Info);


		for (auto& Pair : AS->TagsToAttributes)
		{
			FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
			/*Info.AttributeValue = Pair.Value.Execute().GetNumericValue(AS);*/
			Info.AttributeValue = Pair.Value().GetNumericValue(AS);
			AttributeInfoDelegate.Broadcast(Info);

		}

}
