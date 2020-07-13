// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleState.h"

#include "TimerManager.h"

void AVehicleState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	// If replicated name, we can assume it initialised and can be added to the lobby widget
	ReadyForLobbyWidget();
}


void AVehicleState::StartOldAgressorCheckTimer( float time, const AVehicleState* agressor )
{
	// Create FTimerHandle
	FTimerHandle UniqueHandle;
	// Create an FTimerDelegate, binding it with the agressor
	FTimerDelegate OldAgressorCheckDelegate = FTimerDelegate::CreateUObject(this, &AVehicleState::CheckOldAgressor, agressor);
	
	// Start timer
	GetWorldTimerManager().SetTimer( UniqueHandle, OldAgressorCheckDelegate, time, false );
}
