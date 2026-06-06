// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Player/AuraPlayerState.h>
#include "AbilitySystem/AuraAbilitySystemComponent.h"
//Test
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "DebugHelper.h"
#include <Player/AuraPlayerController.h>
#include <UI/HUD/AuraHUD.h>

//강의엔 추가 안함. 개인 디버그용으로 넣음 
#include "AbilitySystem/AuraAttributeSet.h"

AAuraCharacter::AAuraCharacter()
{



	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	CharacterClass = ECharacterClass::Elementalist;
}
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	//Init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init abiilty actor info for clients
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);


	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}

	}

	InitializeDefaultAttributes();

	if (const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{

	}


}


////Test
//void AAuraCharacter::Tick(float DeltaTime)
//{
//	//강의에선 tick 없음
//	Super::Tick(DeltaTime);
//
//	// Tick에서 Draw DebugArrow 사용
//	DrawDebugDirectionalArrow(
//		GetWorld(),
//		GetActorLocation(),
//		GetActorLocation() + FVector(200.f, 0.f, 0.f), // 월드 X축
//		50.f,
//		FColor::Red,
//		false,
//		-1.f,
//		0,
//		3.f
//	);
//}
////TestEnd


//void AAuraCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
//{
//	Super::FaceRotation(NewControlRotation, DeltaTime);
//	//if (Controller != nullptr)
//	//{
//	//	NewControlRotation.Pitch = 0.f;
//	//	NewControlRotation.Roll = 0.f;
//	//	SetActorRotation(NewControlRotation);
//	//}
//}
