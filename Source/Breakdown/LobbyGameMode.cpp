// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Engine/World.h"

void ALobbyGameMode::PostLogin( APlayerController * NewPlayer )
{
	Super::PostLogin( NewPlayer );
	++m_playerCount;
	if( m_playerCount >= 3 )
	{
		UWorld* World = GetWorld();
		if( !ensure( World != nullptr ) )
		{
			return;
		}
		bUseSeamlessTravel = true;
		
		SetDesiredMap( "/Game/Levels/Quarry_V6" );
		
		World->ServerTravel( m_desiredMap.Append( "?listen" ) );

	}
}

void ALobbyGameMode::Logout( AController * Exiting )
{
	Super::Logout( Exiting );
	--m_playerCount;
}

void ALobbyGameMode::SetDesiredMap( const FString& desiredMap )
{
	m_desiredMap = desiredMap;
}
