#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VehicleState.generated.h"


UCLASS()
class BREAKDOWN_API AVehicleState : public APlayerState
{
	GENERATED_BODY()


public:
	
	void OnRep_PlayerName() override;

	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
		void ReadyForLobbyWidget();

	// Called when an Assist timer is over, this is meant to be implemented on Blueprints
	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
		void AssistCountdownOver( const AVehicleState* agressor );

	// Create an FTimerHandle and start the timer
	UFUNCTION( BlueprintCallable )
		void StartAssistCountdown( float time, const AVehicleState* agressor );

	// Restart a timer with an existing FTimerHandle
	UFUNCTION( BlueprintCallable )
		void ResetAssistCountdown( UPARAM( ref ) FTimerHandle& uniqueHandle, float time, const AVehicleState* agressor );

	// Keep an array of FTimerHandles so we can reset the countdowns when needed
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<FTimerHandle> RecentAgressorTimerHandles;
};
