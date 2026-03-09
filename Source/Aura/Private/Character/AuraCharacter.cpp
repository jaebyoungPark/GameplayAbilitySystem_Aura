// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

//Test
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "DebugHelper.h"

AAuraCharacter::AAuraCharacter()
{



	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}
//Test
void AAuraCharacter::Tick(float DeltaTime)
{
	//강의에선 tick 없음
	Super::Tick(DeltaTime);

	// Tick에서 DrawDebugArrow 사용
	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + FVector(200.f, 0.f, 0.f), // 월드 X축
		50.f,
		FColor::Red,
		false,
		-1.f,
		0,
		3.f
	);
}
//TestEnd


void AAuraCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	Super::FaceRotation(NewControlRotation, DeltaTime);
	//if (Controller != nullptr)
	//{
	//	NewControlRotation.Pitch = 0.f;
	//	NewControlRotation.Roll = 0.f;
	//	SetActorRotation(NewControlRotation);
	//}
}
