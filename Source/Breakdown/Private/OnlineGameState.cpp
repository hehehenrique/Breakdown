// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameState.h"

void AOnlineGameState::AddPlayerState( APlayerState* PlayerState )
{
	Super::AddPlayerState( PlayerState );
	OnPlayerStateAdded( PlayerState );
}

void AOnlineGameState::RemovePlayerState( APlayerState* PlayerState )
{
	OnPlayerStateRemoved( PlayerState );
	Super::RemovePlayerState( PlayerState );
}
