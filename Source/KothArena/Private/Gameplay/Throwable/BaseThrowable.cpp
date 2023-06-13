// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Throwable/BaseThrowable.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABaseThrowable::ABaseThrowable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(MeshComponent);

	TriggerVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetupAttachment(MeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void ABaseThrowable::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		MeshComponent->IgnoreActorWhenMoving(GetOwner(), true);
		TriggerVolume->IgnoreActorWhenMoving(GetOwner(), true);
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseThrowable::OnBeginOverlap);
		ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ABaseThrowable::OnProjectileBounce);
	}

	if (HasAuthority() && bShouldDetonate)
	{
		StartDetonateTimer();
	}

	if (const APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		OwnerController = PawnOwner->GetController();	
	}

	ProjectileMovementComponent->Deactivate();
}

void ABaseThrowable::StartDetonateTimer()
{
	GetWorldTimerManager().SetTimer(DetonateTimerHandle, this, &ABaseThrowable::FireDetonateEvent, TimeToDetonate, false);
}

void ABaseThrowable::SpawnExplodeParticle() const
{
	if (ExplodeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticle, GetActorLocation());
	}
}

void ABaseThrowable::ApplyPointDamage(AActor* OtherActor)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			BaseDamage,
			OwnerController.Get(),
			this,
			DamageType
		);
	}
}

void ABaseThrowable::ApplyRadiusDamage()
{
	TArray<AActor*> IgnoredActors;
	const bool bDamaged = UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		BaseDamage,
		GetActorLocation(),
		DetonateRadius,
		DamageType,
		IgnoredActors,
		this,
		OwnerController.Get()
	);
}

void ABaseThrowable::Launch(const FVector& Direction)
{
	if (!HasAuthority())
	{
		return;
	}

	ProjectileMovementComponent->Activate();
	ProjectileMovementComponent->Velocity = Direction;
}

void ABaseThrowable::Explode()
{
	ApplyRadiusDamage();
	SpawnExplodeParticle();
	Destroy();
}

void ABaseThrowable::Stick(AActor* ActorToStick, UPrimitiveComponent* ComponentToStick, const FName& BoneName)
{
	if (!ActorToStick || !ComponentToStick)
	{
		return;
	}

	const FAttachmentTransformRules StickAttach = FAttachmentTransformRules::KeepWorldTransform;
	AttachToComponent(ComponentToStick, StickAttach, BoneName);
	ComponentToStick->IgnoreActorWhenMoving(this, true);

	ProjectileMovementComponent->Deactivate();
	TriggerVolume->SetGenerateOverlapEvents(false);
}

void ABaseThrowable::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}

	OnHitSomethingEvent(OtherActor, OtherComp);
}

void ABaseThrowable::FireDetonateEvent()
{
	OnEndDetonateTimerEvent();
}

void ABaseThrowable::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	OnProjectileBounceEvent(ImpactResult, ImpactVelocity);
}
