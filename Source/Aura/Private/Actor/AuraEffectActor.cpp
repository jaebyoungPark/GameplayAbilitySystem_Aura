// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "DebugHelper.h"

AAuraEffectActor::AAuraEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();



	
}
void AAuraEffectActor::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	//AB_LOG(LogTemp, Warning, TEXT(" [%d] MapSize=%d"), Num, ActiveEffectHandles.Num());

	Num++;
}
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC  == nullptr) return;


	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);


	//TStart
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();

	AB_LOG(LogTemp, Warning, TEXT("[EffectCauser] : %s, [Instigator] : %s"), *EffectContext->GetEffectCauser()->GetName(), *EffectContext->GetInstigator()->GetName());

	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//FGameplayEffectQuery Query;
	//TArray<FActiveGameplayEffectHandle> Handles = TargetASC->GetActiveEffects(Query);

	//int32 Count = Handles.Num();
	//AB_LOG(LogTemp, Warning, TEXT("ASC Active Effects Count = %d"), Count);

	//AB_LOG(LogTemp, Warning, TEXT("Handle = %s"), *ActiveEffectHandle.ToString());
	//TEnd

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

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

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	//AB_LOG(LogTemp, Warning, TEXT("[InfiniteEffectRemovalPolicy] : %s"), *UEnum::GetValueAsString(InfiniteEffectRemovalPolicy));

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}

		}

		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			//AB_LOG(LogTemp, Warning, TEXT("Removed Handle Valid=%d  MapSize=%d"),
			//	Handle.IsValid(),
			//	ActiveEffectHandles.Num());

			ActiveEffectHandles.FindAndRemoveChecked(Handle);


		}

	}

}






