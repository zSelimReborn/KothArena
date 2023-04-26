// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MathUtils.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UMathUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static float GetAngleBetweenVectors(FVector, FVector);
};
