// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/MathUtils.h"

float UMathUtils::GetAngleBetweenVectors(FVector First, FVector Second)
{
	First.Normalize();
	Second.Normalize();

	const float Cos = FVector::DotProduct(First, Second);
	const float ACos = FMath::Acos(Cos);
	const float Angle = FMath::RadiansToDegrees(ACos);

	return Angle;
}
