// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VehicleState.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API AVehicleState : public APlayerState
{
	GENERATED_BODY()

public:
	
	void OnRep_PlayerName() override;

	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
		void ReadyForLobbyWidget();
};
