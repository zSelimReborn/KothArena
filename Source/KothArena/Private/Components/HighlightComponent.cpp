// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HighlightComponent.h"

#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHighlightComponent::EnableHighlightMesh()
{
	if (!bShouldEnableCustomDepthOnMesh)
	{
		return;
	}
	
	UPrimitiveComponent* Mesh = GetMeshComponent();
	if (Mesh != nullptr)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void UHighlightComponent::DisableHighlightMesh()
{
	if (!bShouldEnableCustomDepthOnMesh)
	{
		return;
	}
	
	UPrimitiveComponent* Mesh = GetMeshComponent();
	if (Mesh != nullptr)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void UHighlightComponent::ShowWidget()
{
	if (!bShouldShowWidgetComponent)
	{
		return;
	}
	
	UWidgetComponent* Widget = GetWidgetComponent();
	if (Widget != nullptr)
	{
		Widget->SetVisibility(true);
	}
}

void UHighlightComponent::HideWidget()
{
	if (!bShouldShowWidgetComponent)
	{
		return;
	}
	
	UWidgetComponent* Widget = GetWidgetComponent();
	if (Widget != nullptr)
	{
		Widget->SetVisibility(false);
	}
}

UPrimitiveComponent* UHighlightComponent::GetMeshComponent()
{
	if (MeshComponent != nullptr)
	{
		return MeshComponent;
	}

	TArray<UActorComponent*> MeshFound = GetOwner()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), MeshComponentTag);
	if (MeshFound.Num() <= 0)
	{
		return nullptr;
	}

	MeshComponent = Cast<UPrimitiveComponent>(MeshFound[0]);
	return MeshComponent;
}

UWidgetComponent* UHighlightComponent::GetWidgetComponent()
{
	if (WidgetComponent != nullptr)
	{
		return WidgetComponent;
	}

	WidgetComponent = GetOwner()->FindComponentByClass<UWidgetComponent>();
	return WidgetComponent;
}

void UHighlightComponent::EnableHighlight()
{
	EnableHighlightMesh();
	ShowWidget();
}

void UHighlightComponent::DisableHighlight()
{
	DisableHighlightMesh();
	HideWidget();
}
