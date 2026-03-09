// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"

#include "DebugHelper.h"


AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(10.f);

	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(250.f);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);

	
}

void AAuraEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	AB_LOG(LogTemp, Warning, TEXT("[CustomDepthStencilValue] : %i, [this name] : %s, %d"), GetMesh()->CustomDepthStencilValue, *this->GetName(), TestVar);

	TestVar++;
}
