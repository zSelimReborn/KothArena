// Copyright Epic Games, Inc. All Rights Reserved.


#include "KothArenaGameModeBase.h"

#include "Characters/BaseCharacter.h"
#include "Controllers/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AKothArenaGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	KillCounterMapping.Empty();
	
	TArray<AActor*> Controllers; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AController::StaticClass(), Controllers);

	for (AActor* ControllerActor : Controllers)
	{
		if (AController* Controller = Cast<AController>(ControllerActor))
		{
			RegisterController(Controller);
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
	if (!KillCounterMapping.Contains(ControllerCauser))
	{
		KillCounterMapping.Add(ControllerCauser, 1);
	}
	else
	{
		KillCounterMapping[ControllerCauser] += 1;
	}

	if (AShooterPlayerController* ShooterController = Cast<AShooterPlayerController>(ControllerCauser))
	{
		ShooterController->OnCharacterKillSomeone(KillCounterMapping[ControllerCauser]);
	}
}

void AKothArenaGameModeBase::OnCharacterBrokenShield(ACharacter* Character, AController* ControllerCauser)
{
	if (AShooterPlayerController* ShooterController = Cast<AShooterPlayerController>(ControllerCauser))
	{
		ShooterController->OnCharacterBrokeShield();
	}
}

void AKothArenaGameModeBase::RegisterController(AController* NewController)
{
	if (KillCounterMapping.Contains(NewController))
	{
		return;
	}
	
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(NewController->GetCharacter()))
	{
		if (!Character->OnTakeDamage().IsAlreadyBound(this, &AKothArenaGameModeBase::OnCharacterTakeDamage))
		{
			Character->OnTakeDamage().AddDynamic(this, &AKothArenaGameModeBase::OnCharacterTakeDamage);
		}
		if (!Character->OnCharacterDeath().IsAlreadyBound(this, &AKothArenaGameModeBase::OnCharacterDeath))
		{
			Character->OnCharacterDeath().AddDynamic(this, &AKothArenaGameModeBase::OnCharacterDeath);
		}
		if (Character->OnCharacterShieldBroken().IsAlreadyBound(this, &AKothArenaGameModeBase::OnCharacterBrokenShield))
		{
			Character->OnCharacterShieldBroken().AddDynamic(this, &AKothArenaGameModeBase::OnCharacterBrokenShield);
		}
		
		KillCounterMapping.Add(NewController, 0);
	}
}
