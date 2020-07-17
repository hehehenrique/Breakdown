// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleState.h"
#include "TimerManager.h"

void AVehicleState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	// If replicated name, we can assume it initialised and can be added to the lobby widget
	ReadyForLobbyWidget();
}


void AVehicleState::StartAssistCountdown( float time, const AVehicleState* agressor )
{
	// Create FTimerHandle
	FTimerHandle uniqueHandle;
	
	RecentAgressorTimerHandles.Emplace( uniqueHandle );
	
	// Create an FTimerDelegate, binding it with the agressor
	FTimerDelegate AssistCountdownDelegate = FTimerDelegate::CreateUObject(this, &AVehicleState::AssistCountdownOver, agressor);
	
	// Start timer
	GetWorldTimerManager().SetTimer( RecentAgressorTimerHandles.Last(), AssistCountdownDelegate, time, false );
}

void AVehicleState::ResetAssistCountdown( UPARAM( ref ) FTimerHandle& uniqueHandle, float time, const AVehicleState* agressor )
{
	// Create an FTimerDelegate, binding it with the agressor
	FTimerDelegate AssistCountdownDelegate = FTimerDelegate::CreateUObject( this, &AVehicleState::AssistCountdownOver, agressor );
		
	// Set uniqueHandle timer with new time
	GetWorldTimerManager().SetTimer( uniqueHandle, AssistCountdownDelegate, time, false );
}
