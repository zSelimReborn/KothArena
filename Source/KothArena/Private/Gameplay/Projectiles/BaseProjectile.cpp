// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Projectiles/BaseProjectile.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Volume"));
	SetRootComponent(TriggerVolume);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(TriggerVolume);
	
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System Component"));
	ParticleSystemComponent->SetupAttachment(TriggerVolume);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ProjectileMovementComponent->SetUpdatedComponent(TriggerVolume);
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
	}
}

void ABaseProjectile::OnProjectileHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ProjectileHitDelegate.Broadcast(this, OtherActor, SweepResult);
	if (bDestroyOnHit)
	{
		Destroy();
	}
}

void ABaseProjectile::Fire(const FVector& Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

