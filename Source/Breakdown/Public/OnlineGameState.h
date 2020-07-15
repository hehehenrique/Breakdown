// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OnlineGameState.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API AOnlineGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/** Add PlayerState to the PlayerArray */
	void AddPlayerState( APlayerState* PlayerState ) override;

	/** Remove PlayerState from the PlayerArray. */
	void RemovePlayerState( APlayerState* PlayerState ) override;

protected:
	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
		void OnPlayerStateAdded( APlayerState* PlayerState );
	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
		void OnPlayerStateRemoved( APlayerState* PlayerState );
};
