// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DebugHelper.h"
#include "Interaction/CombatInterface.h"
#include "Actor/AuraProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	//AB_LOG_NET_INFO(ActorInfo);
}





void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	//AB_LOG_NET_INFO(GetCurrentActorInfo());

	float Test = GetAbilityLevel();

	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	
	/*ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());*/

 	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	//const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}


	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);


	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	
	const FAuraGameplayTags GameplayTag = FAuraGameplayTags::Get();

	for(auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}


	Projectile->DamageEffectSpecHandle = SpecHandle;

	Projectile->FinishSpawning(SpawnTransform);

}
//void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
//{
//	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
//
//	AB_LOG(LogTemp, Warning, TEXT(""));
//
//	const bool bIsServer = HasAuthority(&ActivationInfo);
//	const bool bIsLocallyControlled = 	IsLocallyControlled();
//
//	// 네트워크 역할 문자열
//	const FString AuthorityStr = bIsServer ? TEXT("SERVER") : TEXT("CLIENT");
//	const FString LocalControlStr = bIsLocallyControlled ? TEXT("LocallyControlled") : TEXT("NotLocallyControlled");
//
//	// 액터 이름
//	const FString AvatarName = ActorInfo && ActorInfo->AvatarActor.IsValid()
//		? ActorInfo->AvatarActor->GetName()
//		: TEXT("NullAvatar");
//
//	AB_LOG(LogTemp, Warning, TEXT("  >> Authority   : %s"), *AuthorityStr);
//	AB_LOG(LogTemp, Warning, TEXT("  >> LocalControl: %s"), *LocalControlStr);
//	AB_LOG(LogTemp, Warning, TEXT("  >> Avatar      : %s"), *AvatarName);
//
//
//
//}

