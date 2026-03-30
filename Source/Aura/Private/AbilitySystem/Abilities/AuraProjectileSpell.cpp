// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DebugHelper.h"
#include "Interaction/CombatInterface.h"
#include "Actor/AuraProjectile.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;


	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();


		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		//TODO: Set the Projectile Rotation

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, 
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn

			//TODO: GIve the Projectile a Gameplay Effect Spec for causing Damage.    
		);

		Projectile->FinishSpawning(SpawnTransform);
	}







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

