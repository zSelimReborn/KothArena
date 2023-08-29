// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/Tests/EnvQueryTest_SpotOccupied.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "Subsystems/AIReservationSubsystem.h"

UEnvQueryTest_SpotOccupied::UEnvQueryTest_SpotOccupied(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Medium;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_SpotOccupied::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* DataOwner = QueryInstance.Owner.Get();
	BoolValue.BindData(DataOwner, QueryInstance.QueryID);
	ReservationTolerance.BindData(DataOwner, QueryInstance.QueryID);

	const bool bWantsFreeSpot = BoolValue.GetValue();
	const float Tolerance = ReservationTolerance.GetValue();

	const UAIReservationSubsystem* ReservationSubsystem = GetWorld()->GetSubsystem<UAIReservationSubsystem>();
	if (ReservationSubsystem == nullptr)
	{
		return;
	}
	
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		const bool bCanBeReserved = ReservationSubsystem->SpotCanBeReserved(ItemLocation, Tolerance);
		It.SetScore(TestPurpose, FilterType, bCanBeReserved, bWantsFreeSpot);
	}
}

FText UEnvQueryTest_SpotOccupied::GetDescriptionTitle() const
{
	const FString FreeOrReserved = BoolValue.GetValue()? TEXT("free") : TEXT("reserved");
	
	return FText::FromString(FString::Printf(TEXT("Look for %s positions"), *FreeOrReserved));
}

FText UEnvQueryTest_SpotOccupied::GetDescriptionDetails() const
{
	const FText Details = FText::FromString(FString::Printf(TEXT("using a tolerance of %.2f to check reserved spots."), ReservationTolerance.GetValue()));
	return Details;
}
