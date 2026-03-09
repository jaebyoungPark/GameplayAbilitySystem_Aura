// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "MySpringArmComponent.generated.h"

/**
 * 
 */

 //나만의 테스트를 위해 추가한 스프링암 컴포넌트입니다.
UCLASS(meta = (BlueprintSpawnableComponent))
class AURA_API UMySpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
public:
	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

	FRotator GetTargetRotation() const;

	//void UpdateChildTransforms(EUpdateTransformFlags UpdateTransformFlags = EUpdateTransformFlags::None, ETeleportType Teleport = ETeleportType::None);

};
