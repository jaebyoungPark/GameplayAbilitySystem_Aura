// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCameraComponent.h"
#include "GameFramework/SpringArmComponent.h" 
#include "DebugHelper.h"

//void UMyCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
//{
//	Super::GetCameraView(DeltaTime, DesiredView);
//	//AA_LOG(LogTemp, Warning, TEXT(""));
//}
//
void UMyCameraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	////AB_LOG(LogTemp, Warning, TEXT("[WorldLocation] : %s"), *GetComponentLocation().ToString());
	const FTransform RelativeTransform(RelativeRotationCache.RotatorToQuat(GetRelativeRotation()), GetRelativeLocation(), GetRelativeScale3D());


    USceneComponent* Parent = GetAttachParent();
    if (Parent)
    {
        //AB_LOG(LogTemp, Warning, TEXT("Parent Component : %s"), *Parent->GetName());
    }
    else
    {
        //AB_LOG(LogTemp, Warning, TEXT("No Parent Component attached"));
    }

    FTransform NewTransform = CalcNewComponentToWorld(RelativeTransform, Parent, GetAttachSocketName());

    //AB_LOG(LogTemp, Warning, TEXT("[RelativeTransform] : %s, [NewTransform] : %s"), *RelativeTransform.ToString(), *NewTransform.ToString());
}

