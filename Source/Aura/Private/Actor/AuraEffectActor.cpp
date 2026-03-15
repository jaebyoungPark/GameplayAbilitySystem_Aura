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

	// Apply 후 Stack 상태 디버그 출력
	TArray<FActiveGameplayEffectHandle> ActiveHandles =
		TargetASC->GetActiveEffects(FGameplayEffectQuery());

	for (const FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		const FActiveGameplayEffect* ActiveGE = TargetASC->GetActiveGameplayEffect(Handle);
		if (ActiveGE)
		{
			float StartTime = ActiveGE->StartWorldTime;
			float CurrentTime = GetWorld()->GetTimeSeconds();
			float ElapsedTime = CurrentTime - StartTime;

			UE_LOG(LogTemp, Warning,
				TEXT("GE: %s | Stack: %d | ElapsedTime: %.7f | Source ASC: %s"),
				*ActiveGE->Spec.Def->GetName(),
				ActiveGE->Spec.StackCount,
				ElapsedTime,
				*GetNameSafe(ActiveGE->Spec.GetContext().GetInstigatorAbilitySystemComponent()));
		}
		else
		{
			AB_LOG(LogTemp, Warning, TEXT(""));
		}
	}

}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
}






