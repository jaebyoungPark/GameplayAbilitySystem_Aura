// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "Engine/LocalPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);
	
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
