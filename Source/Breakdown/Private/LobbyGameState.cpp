// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

void ALobbyGameState::AddPlayerState( APlayerState* PlayerState )
{
	Super::AddPlayerState( PlayerState );
	OnPlayerStateAdded( PlayerState );
}

void ALobbyGameState::RemovePlayerState( APlayerState* PlayerState )
{
	OnPlayerStateRemoved( PlayerState );
	Super::RemovePlayerState( PlayerState );
}
