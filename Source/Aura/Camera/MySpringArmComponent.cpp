// Fill out your copyright notice in the Description page of Project Settings.


#include "MySpringArmComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

#include "DebugHelper.h"


//void UMySpringArmComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//
//
//	////AA_LOG(LogTemp, Warning, TEXT("[RelativeRotation] : %s"), *GetRelativeRotation().ToString());
//
//
//
//}
//
FRotator UMySpringArmComponent::GetTargetRotation() const
{

	FRotator DesiredRot = GetDesiredRotation();

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	FRotator ViewRotation = PawnOwner ? PawnOwner->GetViewRotation() : FRotator::ZeroRotator;

	//AB_LOG(LogTemp, Warning, TEXT("[DesiredRot (ComponentRotation)] : %s, [bUsePawnControlRotation] : %d, [ViewRotation] : %s"), *DesiredRot.ToString(), bUsePawnControlRotation, *ViewRotation.ToString());

	if (bUsePawnControlRotation)
	{
		if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			const FRotator PawnViewRotation = OwningPawn->GetViewRotation();
			if (DesiredRot != PawnViewRotation)
			{
				DesiredRot = PawnViewRotation;
			}
		}
	}

	// If inheriting rotation, check options for which components to inherit
	if (!IsUsingAbsoluteRotation())
	{
		//AB_LOG(LogTemp, Warning, TEXT("[bInheritPitch] : %d, [bInheritYaw] : %d, [bInheritRoll] : %d"), bInheritPitch, bInheritYaw, bInheritRoll);

		const FRotator LocalRelativeRotation = GetRelativeRotation();
		//AB_LOG(LogTemp, Warning, TEXT("[LocalRelativeRotation] : %s"), *LocalRelativeRotation.ToString());
		if (!bInheritPitch)
		{
			DesiredRot.Pitch = LocalRelativeRotation.Pitch;
		}

		if (!bInheritYaw)
		{
			DesiredRot.Yaw = LocalRelativeRotation.Yaw;
		}

		if (!bInheritRoll)
		{
			DesiredRot.Roll = LocalRelativeRotation.Roll;
		}
	}
	//AB_LOG(LogTemp, Warning, TEXT("[Final DesiredRot] : %s"), *DesiredRot.ToString());
	return DesiredRot;
}

void UMySpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	//AA_LOG(LogTemp, Warning, TEXT("======================================"));
	
	


	//Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);

	FRotator DesiredRot = GetTargetRotation();

	// If our viewtarget is simulating using physics, we may need to clamp deltatime
	if (bClampToMaxPhysicsDeltaTime)
	{
		// Use the same max timestep cap as the physics system to avoid camera jitter when the viewtarget simulates less time than the camera
		DeltaTime = FMath::Min(DeltaTime, UPhysicsSettings::Get()->MaxPhysicsDeltaTime);
	}

	// Apply 'lag' to rotation if desired
	if (bDoRotationLag)
	{
		//AA_LOG(LogTemp, Warning, TEXT(""));
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f)
		{
			const FRotator ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (1.f / DeltaTime);
			FRotator LerpTarget = PreviousDesiredRot;
			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmRotStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(LerpTarget), LerpAmount, CameraRotationLagSpeed));
				PreviousDesiredRot = DesiredRot;
			}
		}
		else
		{
			DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(DesiredRot), DeltaTime, CameraRotationLagSpeed));
		}
	}
	PreviousDesiredRot = DesiredRot;

	//AA_LOG(LogTemp, Warning, TEXT("[TargetOffset] : %s, [ComponentLocation] : %s"),*TargetOffset.ToString(),  *GetComponentLocation().ToString());

	// Get the spring arm 'origin', the target we want to look at
	FVector ArmOrigin = GetComponentLocation() + TargetOffset;

	//AA_LOG(LogTemp, Warning, TEXT("[ArmOrigin] : %s"), *ArmOrigin.ToString());
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector DesiredLoc = ArmOrigin;
	if (bDoLocationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraLagSpeed > 0.f)
		{
			const FVector ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
			FVector LerpTarget = PreviousDesiredLoc;

			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, CameraLagSpeed);
				PreviousDesiredLoc = DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
			
		}

		// Clamp distance if requested
		bool bClampedDist = false;
		if (CameraLagMaxDistance > 0.f)
		{
			const FVector FromOrigin = DesiredLoc - ArmOrigin;
			if (FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance))
			{
				DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
				bClampedDist = true;
			}
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (bDrawDebugLagMarkers)
		{
			DrawDebugSphere(GetWorld(), ArmOrigin, 5.f, 8, FColor::Green);
			DrawDebugSphere(GetWorld(), DesiredLoc, 5.f, 8, FColor::Yellow);

			const FVector ToOrigin = ArmOrigin - DesiredLoc;
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
		}
#endif
	}

	PreviousArmOrigin = ArmOrigin;
	PreviousDesiredLoc = DesiredLoc;

	// Now offset camera position back along our rotation
	DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
	//AA_LOG(LogTemp, Warning, TEXT("[DesiredRot.Vector() * TargetArmLength] : %s, [DesiredRot.Vector()] : %s, [DesiredLoc] : %s"), *(DesiredRot.Vector() * TargetArmLength).ToString(), *DesiredRot.Vector().ToString(), *DesiredLoc.ToString());

	// Add socket offset in local space
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);




	// Do a sweep to ensure we are not penetrating the world
	FVector ResultLoc;
	if (bDoTrace && (TargetArmLength != 0.0f))
	{
		bIsCameraFixed = true;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

		FHitResult Result;
		GetWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);

		UnfixedCameraPosition = DesiredLoc;

		ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);

		if (ResultLoc == DesiredLoc)
		{
			bIsCameraFixed = false;
		}
	}
	else
	{
		ResultLoc = DesiredLoc;
		bIsCameraFixed = false;
		UnfixedCameraPosition = ResultLoc;
	}

	// Form a transform for new world transform for camera
	FTransform WorldCamTM(DesiredRot, ResultLoc);
	//AA_LOG(LogTemp, Warning, TEXT("[WorldCamTM] : %s, [bComponentToWorldUpdated] : %d"), *WorldCamTM.ToString(), bComponentToWorldUpdated);
	// Convert to relative to component
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());
	//AA_LOG(LogTemp, Warning, TEXT("[RelCamTM] : %s, [ComponentTransform] : %s"), *RelCamTM.ToString(), *GetComponentTransform().ToString());

	// Update socket location/rotation
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();

	//AA_LOG(LogTemp, Warning, TEXT("[RelativeSocketLocation] : %s, [RelativeSocketRotation] : %s"), *RelativeSocketLocation.ToString(), *RelativeSocketRotation.ToString());

	UpdateChildTransforms();
}

//void UMySpringArmComponent::UpdateChildTransforms(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
//{
//	////AB_LOG(LogTemp, Warning, TEXT("[bComponentToWorldUpdated] : %s"), bComponentToWorldUpdated ? TEXT("True") : TEXT("False"));
//
//#if ENABLE_NAN_DIAGNOSTIC
//	if (!GetComponentTransform().IsValid())
//	{
//		logOrEnsureNanError(TEXT("USceneComponent::UpdateChildTransforms found NaN/INF in ComponentToWorld: %s"), *GetComponentTransform().ToString());
//	}
//#endif
//
//	if (AttachChildren.Num() > 0)
//	{
//		const bool bOnlyUpdateIfUsingSocket = !!(UpdateTransformFlags & EUpdateTransformFlags::OnlyUpdateIfUsingSocket);
//
//		const EUpdateTransformFlags UpdateTransformNoSocketSkip = ~EUpdateTransformFlags::OnlyUpdateIfUsingSocket & UpdateTransformFlags;
//		const EUpdateTransformFlags UpdateTransformFlagsFromParent = UpdateTransformNoSocketSkip | EUpdateTransformFlags::PropagateFromParent;
//
//
//		for (USceneComponent* ChildComp : GetAttachChildren())
//		{
//
//			if (ChildComp != nullptr)
//			{
//
//				//AB_LOG(LogTemp, Warning, TEXT("[ChildComp->bComponentToWorldUpdated] : %s"), ChildComp->bComponentToWorldUpdated ? TEXT("True") : TEXT("False"));
//				//AB_LOG(LogTemp, Warning, TEXT("[ChildComp->GetAttachSocketName()] : %s"), *ChildComp->GetAttachSocketName().ToString());
//
//				// Update Child if it's never been updated.
//				if (!ChildComp->bComponentToWorldUpdated)
//				{
//
//					ChildComp->UpdateComponentToWorld(UpdateTransformFlagsFromParent, Teleport);
//				}
//				else
//				{
//					// If we're updating child only if it's using a socket. Skip if that's not the case.
//					if (bOnlyUpdateIfUsingSocket && (ChildComp->AttachSocketName == NAME_None))
//					{
//						//AB_LOG(LogTemp, Warning, TEXT("bOnlyUpdateIfUsingSocket && (ChildComp->AttachSocketName == NAME_None)"));
//
//						continue;
//					}
//
//					// Don't update the child if it uses a completely absolute (world-relative) scheme.
//					if (ChildComp->IsUsingAbsoluteLocation() && ChildComp->IsUsingAbsoluteRotation() && ChildComp->IsUsingAbsoluteScale())
//					{
//						//AB_LOG(LogTemp, Warning, TEXT("ChildComp->IsUsingAbsoluteLocation() && ChildComp->IsUsingAbsoluteRotation() && ChildComp->IsUsingAbsoluteScale()"));
//
//						continue;
//					}
//
//					const FTransform RelativeTransform(ChildComp->RelativeRotationCache.RotatorToQuat(ChildComp->GetRelativeRotation()), ChildComp->GetRelativeLocation(), ChildComp->GetRelativeScale3D());
//
//					//AB_LOG(LogTemp, Warning, TEXT("[RelativeTransform] : %s, [ChildComp->GetAttachSocketName] : %s"), *RelativeTransform.ToString(), *ChildComp->GetAttachSocketName().ToString());
//
//
//		/*			ChildComp->CalcNewComponentToWorld(RelativeTransform, this, ChildComp->GetAttachSocketName());*/
//
//
//
//					ChildComp->UpdateComponentToWorld(UpdateTransformFlagsFromParent, Teleport);
//				}
//			}
//		}
//	}
//}