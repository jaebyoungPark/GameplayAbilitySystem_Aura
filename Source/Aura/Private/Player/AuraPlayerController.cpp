// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

#include "DebugHelper.h"

//Test
#include "Aura/Camera/MyPlayerCameraManager.h"

AAuraPlayerController::AAuraPlayerController()
{	
	bReplicates = true;

	//테스트용
	PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass();
}

void AAuraPlayerController::BeginPlay()
{


	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);


}

void AAuraPlayerController::SetupInputComponent()
{


	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (EnhancedInputComponent)
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	}



}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

}

void AAuraPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);


}

void AAuraPlayerController::Move(const FInputActionValue& Value)
{
	
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	/*UE_LOG(LogTemp, Warning, TEXT("[YawRotation] : %s, [Rotation] : %s, [InputAxisVector]: %s, [ForwardDirection] : %s, [RightDirection] : %s"), *YawRotation.ToString(), *Rotation.ToString(), *InputAxisVector.ToString(), *ForwardDirection.ToString(), *RightDirection.ToString());*/

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

	}


}

void AAuraPlayerController::CursorTrace()
{
	//AB_LOG(LogTemp, Warning, TEXT("[ThisActor] : %s, [LastActor] : %s"),
	//	*GetNameSafe(ThisActor.GetObject()),
	//	*GetNameSafe(LastActor.GetObject())
	//);

	FHitResult CursorHit;

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if ( !CursorHit.bBlockingHit )
	{
	

		//AB_LOG(LogTemp, Warning, TEXT("BlockingHit is Invalid %d"), Test);



		return;
	}

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();
	
	/**
	* Line trace from cursor. There are several scenarios:
	*  A. LastActor is null && ThisActor is null
	*		 - Do nothing
	*  B. Last Actor is null && ThisActor is valid
	*		 - Highlight ThisActor
	*  C. Last Actor is valid && ThisActor is null
	*		- UnHighlight LastActor
	*	D. Both Actor are valid, but LastActor != ThisActor
	*		- UnHighlight LastActor, and Highlight ThisActor
	*	E. Both Actors are valid, and are the same Actor
	*		- Do nothing
	*/


	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case B
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A - Both are null, do nothing 
		}
	}
	else // LastActor is valid 
	{
		if (ThisActor == nullptr)
		{
			// Case C 
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != ThisActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Cas E - do nothing
			}


			
		}

	}

}
