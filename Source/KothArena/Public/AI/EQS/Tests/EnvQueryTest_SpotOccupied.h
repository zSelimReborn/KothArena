// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_SpotOccupied.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UEnvQueryTest_SpotOccupied : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_SpotOccupied(const FObjectInitializer& ObjectInitializer);

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	
	virtual FText GetDescriptionDetails() const override;

protected:
	UPROPERTY(EditAnywhere, Category=Reservation, AdvancedDisplay)
	FAIDataProviderFloatValue ReservationTolerance;
};
