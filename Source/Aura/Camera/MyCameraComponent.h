// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "MyCameraComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class AURA_API UMyCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	//virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};