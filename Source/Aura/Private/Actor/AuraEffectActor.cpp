// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "DebugHelper.h"

AAuraEffectActor::AAuraEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	
}
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	AB_LOG(LogTemp, Warning, TEXT(" Begin"));

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC  == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Source Actor: %s | Address: %p"), *GetName(), this);
	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());



}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
}






