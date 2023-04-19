// Copyright Epic Games, Inc. All Rights Reserved.


#include "KothArenaGameModeBase.h"

#include "Characters/BaseCharacter.h"
#include "Controllers/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AKothArenaGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Characters; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), Characters);

	for (AActor* Actor : Characters)
	{
		if (ABaseCharacter* Character = Cast<ABaseCharacter>(Actor))
		{
			Character->OnTakeDamage().AddDynamic(this, &AKothArenaGameModeBase::OnCharacterTakeDamage);
			Character->OnCharacterDeath().AddDynamic(this, &AKothArenaGameModeBase::OnCharacterDeath);
		}
	}
}

void AKothArenaGameModeBase::OnCharacterTakeDamage(AController* ControllerDamaged, AController* ControllerCauser)
{
	if (AShooterPlayerController* ShooterController = Cast<AShooterPlayerController>(ControllerCauser))
	{
		ShooterController->OnCharacterHitSomeone();
	}
}

void AKothArenaGameModeBase::OnCharacterDeath(ACharacter* Character, AController* ControllerCauser)
{
	if (AShooterPlayerController* ShooterController = Cast<AShooterPlayerController>(ControllerCauser))
	{
		ShooterController->OnCharacterKillSomeone();
	}
}
