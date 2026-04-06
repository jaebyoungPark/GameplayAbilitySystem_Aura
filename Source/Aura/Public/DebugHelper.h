// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)


namespace Debug
{
	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 3.f, Color, Msg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}

	static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			const FString FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(InKey, 99.f, Color, FinalMsg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMsg);
		}
	}
}

#define DEBUG_PRINT(Msg) \
    Debug::Print(FString::Printf(TEXT("[%s] %s"), LOG_CALLINFO, *FString(Msg)))




/**
 * In World 
 */
//Version 1
#define AB_LOG(LogCat, Verbosity, Format, ...) \
do { \
    if (GWorld && GWorld->IsGameWorld()) \
    { \
        UE_LOG(LogCat, Verbosity, TEXT("%-70s %s"), \
        LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)); \
    } \
} while(0)




#define AB_LOG_NET_INFO(ActorInfo) \
do { \
    const TCHAR* NetRole = (ActorInfo)->IsNetAuthority() ? TEXT("Server (Authority)") : TEXT("Client"); \
    const TCHAR* LocalRole = (ActorInfo)->IsLocallyControlled() ? TEXT("Locally Controlled") : TEXT("Not Locally Controlled"); \
    AB_LOG(LogTemp, Warning, TEXT("%s | %s"), NetRole, LocalRole); \
} while(0)




#define AB_LOG_NET_INFO_ACTOR(Actor) \
do { \
    const TCHAR* NetRole = (Actor)->HasAuthority() ? TEXT("Server") : TEXT("Client"); \
    AActor* OwnerActor = (Actor)->GetOwner(); \
    AController* OwnerController = Cast<AController>(OwnerActor); \
    if (!OwnerController) \
    { \
        if (APawn* OwnerPawn = Cast<APawn>(OwnerActor)) \
        { \
            OwnerController = OwnerPawn->GetController(); \
        } \
    } \
    int32 PlayerID = -1; \
    if (OwnerController) \
    { \
        if (APlayerState* PS = OwnerController->GetPlayerState<APlayerState>()) \
        { \
            PlayerID = PS->GetPlayerId(); \
        } \
    } \
    AB_LOG(LogTemp, Warning, TEXT("[%s][%s][PlayerID:%d] Owner:%s / Controller:%s"), \
        NetRole, \
        *GetNameSafe(Actor), \
        PlayerID, \
        *GetNameSafe(OwnerActor), \
        *GetNameSafe(OwnerController)); \
} while(0)


////±‚¡∏
//#define AB_LOG(LogCat, Verbosity, Format, ...) \
//    UE_LOG(LogCat, Verbosity, TEXT("%-70s %s"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))




//Debugging 
void DrawAutoRunDebug(UWorld* World, const FVector& PawnLocation, const FVector& LocationOnSpline, const FVector& Direction);