// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
//기존
//#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
//#define AA_LOG(LogCat, Verbosity, Format, ...) \
//UE_LOG(LogCat, Verbosity, TEXT("%-70s %s"), \
//LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))



#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define AB_LOG(LogCat, Verbosity, Format, ...) \
    if (GWorld && GWorld->IsGameWorld()) \
    { \
        UE_LOG(LogCat, Verbosity, TEXT("%-70s %s"), \
        LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)) \
    }


// 새로운 매크로 이름
#define AA_LOG(LogCat, Verbosity, Format, ...) \
    do { \
        if (GWorld && GWorld->IsGameWorld()) \
        { \
            UE_LOG(LogCat, Verbosity, Format, ##__VA_ARGS__); \
        } \
    } while(0)