// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

#include "DebugHelper.h"

//Test
#include "Aura/Camera/MyPlayerCameraManager.h"

AAuraPlayerController::AAuraPlayerController()
{	
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

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

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();

}
void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}	
}
void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{

	
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}


}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{


	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();


		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}

	}

}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	/*AB_LOG(LogTemp, Warning, TEXT(""));*/

	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}


	/*AB_LOG(LogTemp, Warning, TEXT("[GetASC()] : %s"), GetASC() ? TEXT("True") : TEXT("False"));*/

	if (GetASC())	GetASC()->AbilityInputTagReleased(InputTag);


	if (!bTargeting && !bShiftKeyDown)
	{
		/*AB_LOG(LogTemp, Warning, TEXT("if (!bTargeting && !bShiftKeyDown) == true"));*/

		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			/*AB_LOG(LogTemp, Warning, TEXT("FollowTime is Less Than ShortPressThreshold"));*/

			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}

				/*AB_LOG(LogTemp, Warning, TEXT("[PathPoints.Num()] : %d"), NavPath->PathPoints.Num());*/
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}

			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}

}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld); 

}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		/*AB_LOG(LogTemp, Warning, TEXT("bAutoRunning is true"));*/
		const FVector PawnLocation = ControlledPawn->GetActorLocation();

		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

		//Debug
		DrawAutoRunDebug(GetWorld(), PawnLocation, LocationOnSpline, Direction);

		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}

	}
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

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if ( !CursorHit.bBlockingHit )
	{
		return;
	}

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}

	////Deprecated Code
	//if (LastActor == nullptr)
	//{
	//	if (ThisActor != nullptr)
	//	{

	//		ThisActor->HighlightActor();
	//	}
	//	else
	//	{

	//	}
	//}
	//else 
	//{
	//	if (ThisActor == nullptr)
	//	{
	//	
	//		LastActor->UnHighlightActor();
	//	}
	//	else 
	//	{
	//		if (LastActor != ThisActor)
	//		{
	//		
	//			LastActor->UnHighlightActor();
	//			ThisActor->HighlightActor();
	//		}
	//		else
	//		{
	//			
	//		}

	//	}

	//}

}


