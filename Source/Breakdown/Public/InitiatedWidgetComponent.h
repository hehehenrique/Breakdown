// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InitiatedWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API UInitiatedWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	
	void InitWidget() override;

	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
	void Initialise();
};
