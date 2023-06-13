// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

class UWidgetComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, ToolTip="Give player visual feedback enabling custom depth and custom widget on owner.") )
class KOTHARENA_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHighlightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void EnableHighlightMesh();
	void DisableHighlightMesh();

	void ShowWidget();
	void HideWidget();

	UPrimitiveComponent* GetMeshComponent();
	UWidgetComponent* GetWidgetComponent();
	
// Component interface
public:
	void EnableHighlight();
	void DisableHighlight();
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Highlight")
	bool bShouldEnableCustomDepthOnMesh = true;
	
	UPROPERTY(EditAnywhere, Category="Highlight")
	FName MeshComponentTag;

	UPROPERTY(EditAnywhere, Category="Highlight")
	bool bShouldShowWidgetComponent = true;

	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> MeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> WidgetComponent;
};
