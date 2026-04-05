#include "DebugHelper.h"


void DrawAutoRunDebug(UWorld* World, const FVector& PawnLocation, const FVector& LocationOnSpline, const FVector& Direction)
{

	if (!World) return;

	// 🔴 Pawn 위치 (빨간 점)
	DrawDebugSphere(
		World,
		PawnLocation,
		10.f,
		12,
		FColor::Red
	);

	// 🔵 스플라인 위 가장 가까운 점 (검은 점)
	DrawDebugSphere(
		World,
		LocationOnSpline,
		12.f,
		12,
		FColor::Black
	);

	// 🟢 방향 벡터
	DrawDebugLine(
		World,
		LocationOnSpline,
		LocationOnSpline + Direction * 100.f,
		FColor::Green,
		false,
		0.f,
		0,
		2.f
	);

	// 🟡 Pawn → Spline 연결선
	DrawDebugLine(
		World,
		PawnLocation,
		LocationOnSpline,
		FColor::Yellow,
		false,
		0.f,
		0,
		1.f
	);
}
