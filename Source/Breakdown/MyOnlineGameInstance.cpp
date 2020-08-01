
#include "MyOnlineGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/Overlay.h"
#include "EBreakdownGameMode.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"

// Server Settings FName's
const static FName SESSION_NAME = TEXT( "BCU Breakdown" );
const static FName GAMEMODE_SESSION_KEY = TEXT( "EBreakdownGameMode" );
const static FName SERVER_NAME_KEY = TEXT( "ServerName" );
const static FName CURRENT_PLAYERS_KEY = TEXT( "1" );

UMyOnlineGameInstance::UMyOnlineGameInstance( const FObjectInitializer& ObjectInitializer )
	: UGameInstance( ObjectInitializer )
	, desiredMap( "/Game/Levels/Quarry_V6" )
	, m_pMainMenu( nullptr )
{}

void UMyOnlineGameInstance::Init()
{
	UE_LOG( LogTemp, Warning, TEXT( "Found class %s" ), *MainMenuClass->GetName() );
	IOnlineSubsystem* pOnlineSubsystem = IOnlineSubsystem::Get();
	if( pOnlineSubsystem != nullptr )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Found online subsystem %s" ), *pOnlineSubsystem->GetSubsystemName().ToString() );
		m_pSessionInterface = pOnlineSubsystem->GetSessionInterface();
		if( m_pSessionInterface.IsValid() )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Found session interface" ) );
			m_pSessionInterface->OnCreateSessionCompleteDelegates.AddUObject( this, &UMyOnlineGameInstance::OnCreateSessionComplete );
			m_pSessionInterface->OnDestroySessionCompleteDelegates.AddUObject( this, &UMyOnlineGameInstance::OnDestroySessionComplete );
			m_pSessionInterface->OnFindSessionsCompleteDelegates.AddUObject( this, &UMyOnlineGameInstance::OnFindSessionsComplete );
			m_pSessionInterface->OnJoinSessionCompleteDelegates.AddUObject( this, &UMyOnlineGameInstance::OnJoinSessionComplete );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No online subsystem is configured" ) );
	}
}

UMainMenu* UMyOnlineGameInstance::LoadOnlineMenu()
{
	if( m_pMainMenu == nullptr )
	{
		if( !ensure( MainMenuClass != nullptr ) )
		{
			return nullptr;
		}

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if( !ensure( PlayerController != nullptr ) )
		{
			return nullptr;
		}
		m_pMainMenu = CreateWidget < UMainMenu >( this, MainMenuClass );
		m_pMainMenu->bIsFocusable = true;

		m_pMainMenu->SetMenuInterface( this );
	}
	m_pMainMenu->Setup();

	return m_pMainMenu;
}

void UMyOnlineGameInstance::Host( const FServerData& serverData )
{
	m_hostingServerData = serverData;
	if( m_pSessionInterface.IsValid() )
	{
		CreateSession( serverData );
	}
}

void UMyOnlineGameInstance::OnCreateSessionComplete_Implementation( FName sessionName, bool success )
{
	if( !success )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Could not create session." ) );
		return;
	}

	UEngine* Engine = GetEngine();
	if( !ensure( Engine != nullptr ) )
	{
		return;
	}
	Engine->AddOnScreenDebugMessage( 0, 2, FColor::Green, TEXT( "Hosting" ) );
	UWorld* World = GetWorld();
	if( !ensure( World != nullptr ) )
	{
		return;
	}
	// ServerTravel to specified map with the option "listen", so that it works as a listen server. This way it can receive information from clients
	World->ServerTravel( desiredMap.Append( "?listen" ) );
}

void UMyOnlineGameInstance::OnDestroySessionComplete_Implementation( FName sessionName, bool success )
{
	if( success )
	{
		CreateSession( m_hostingServerData );
	}
}

void UMyOnlineGameInstance::RefreshServerList()
{
	m_pSessionInterface->CancelFindSessions();
	UE_LOG( LogTemp, Warning, TEXT( "UMyOnlineGameInstance::RefreshServerList" ) );

	// Clear the list and activate loading animation
	m_pMainMenu->GetServerList()->ClearChildren();
	m_pMainMenu->GetFindingServersOverlay()->SetVisibility( ESlateVisibility::Visible );

	m_pSessionSearch = MakeShareable( new FOnlineSessionSearch() );
	if( m_pSessionSearch.IsValid() )
	{
		// Set Search Mode ( LAN or Online Subsystem? )
		m_pSessionSearch->bIsLanQuery = m_pMainMenu->GetSearchMode() == 1;

		m_pSessionSearch->MaxSearchResults = 20;

		// Look for servers that use Presence
		m_pSessionSearch->QuerySettings.Set( SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals );

		UE_LOG( LogTemp, Warning, TEXT( "Started Finding Sessions" ) );

		m_pSessionInterface->FindSessions( 0, m_pSessionSearch.ToSharedRef() );
	}
}

void UMyOnlineGameInstance::DestroySession()
{
	if( m_pSessionInterface != nullptr )
		m_pSessionInterface->DestroySession( SESSION_NAME );
}

void UMyOnlineGameInstance::OnFindSessionsComplete_Implementation( bool success )
{
	if( success && m_pSessionSearch.IsValid() && m_pMainMenu != nullptr )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Finished Finding Sessions.!!!" ) );
		UE_LOG( LogTemp, Warning, TEXT( "%d" ), m_pSessionSearch->SearchResults.Num() );

		// Array of FServerData that we will use to create our server list
		TArray<FServerData> serverDatas;

		// For loop to configure each serverData and populate the array
		for( const FOnlineSessionSearchResult& searchResult : m_pSessionSearch->SearchResults )
		{
			// Bool to make sure this is a valid server, and should be added into the list
			bool isValidServer = true;

			UE_LOG( LogTemp, Warning, TEXT( "Found Session: %s" ), *searchResult.GetSessionIdStr() );

			// Create a new serverData struct
			FServerData serverData;

			// Set struct variables
			serverData.name = searchResult.GetSessionIdStr();
			serverData.maxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			serverData.hostUsername = searchResult.Session.OwningUserName;

			TSharedPtr < const FUniqueNetId > hostUserID = searchResult.Session.OwningUserId;

			//CSteamID hostSteamID ( dynamic_cast<uint64>( *hostUserID->ToString() ) );
			UE_LOG( LogTemp, Warning, TEXT( "Host ID: %s" ), *hostUserID->ToString() );
			if( IOnlineSubsystem::Get()->GetSubsystemName() != "NULL" )
			{
				int32 steamID = ( FCString::Atoi( *hostUserID->ToString() ) );
				UE_LOG( LogTemp, Warning, TEXT( "Steam ID: %d" ), steamID );
			}


			// Get the CurrentPlayers key and set the relevant struct var
			const FOnlineSessionSetting* pCurrentPlayersKey = searchResult.Session.SessionSettings.Settings.Find( CURRENT_PLAYERS_KEY );
			// If found key
			if( pCurrentPlayersKey )
			{
				int32 currentPlayers;
				pCurrentPlayersKey->Data.GetValue( currentPlayers );
				serverData.currentPlayers = currentPlayers;
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Could not find current players." ) );
				isValidServer = false;
			}

			// Get the GameMode key and set the relevant struct var
			const FOnlineSessionSetting* pGameModeKey = ( searchResult.Session.SessionSettings.Settings.Find( GAMEMODE_SESSION_KEY ) );

			// If found key
			if( pGameModeKey )
			{
				int32 serverGameMode;
				pGameModeKey->Data.GetValue( serverGameMode );
				serverData.gameMode = static_cast< EBreakdownGameMode >( serverGameMode );
				UE_LOG( LogTemp, Warning, TEXT( "Found server game mode: %d" ), serverData.gameMode );
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Could not find gameMode." ) );
				isValidServer = false;
			}

			// Get the ServerName key and set the relevant struct var
			const FOnlineSessionSetting* pServerNameKey = ( searchResult.Session.SessionSettings.Settings.Find( SERVER_NAME_KEY ) );

			// If found key
			if( pServerNameKey )
			{
				FString serverName;
				pServerNameKey->Data.GetValue( serverName );
				serverData.name = serverName;
				UE_LOG( LogTemp, Warning, TEXT( "Found server name: %s" ), *serverName );
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Could not find server name." ) );
				isValidServer = false;
			}

			// Finally, if server is valid, insert in server list
			if( isValidServer )
			{
				serverDatas.Add( serverData );
			}
		}
		// Populate the server list
		m_pMainMenu->CreateServerList( serverDatas );
		// Hide the "loading" throbber
		m_pMainMenu->GetFindingServersOverlay()->SetVisibility( ESlateVisibility::Collapsed );
	}
}

void UMyOnlineGameInstance::OnJoinSessionComplete( FName sessionName, EOnJoinSessionCompleteResult::Type result )
{
	UE_LOG( LogTemp, Warning, TEXT( "OnJoinSessionComplete called" ) );
	switch( result )
	{
	case EOnJoinSessionCompleteResult::Success:
	{

		if( !m_pSessionInterface.IsValid() ) return;

		FString address;
		if( !m_pSessionInterface->GetResolvedConnectString( sessionName, address ) )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Could not get connect string" ) );
			return;
		}

		UEngine* Engine = GetEngine();

		if( !ensure( Engine != nullptr ) )	return;

		APlayerController* PlayerController = GetFirstLocalPlayerController();

		if( !ensure( PlayerController != nullptr ) ) return;

		Engine->AddOnScreenDebugMessage( 0, 5, FColor::Green, FString::Printf( TEXT( "OnJoinSessionComplete called. Joining address %s" ), *address ) );

		UE_LOG( LogTemp, Warning, TEXT( "OnJoinSessionComplete called. ClientTravel will be called in next line. Joining address %s" ), *address );

		// Perform the ClientTravel to the server address in order to join it
		PlayerController->ClientTravel( ( address ), ETravelType::TRAVEL_Absolute );
	}
	break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
	{
		ConnectionAttemptFailed( EConnectionError::InvalidSession );
	}
	break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
	{
		ConnectionAttemptFailed( EConnectionError::SessionIsFull );
	}
	break;
	default:
	{
		ConnectionAttemptFailed( EConnectionError::Unknown );
	}
	break;
	}
}

void UMyOnlineGameInstance::UpdateCurrentPlayers( int currentPlayers )
{
	if( m_pSessionInterface.IsValid() )
	{
		FOnlineSessionSettings sessionSettings = *m_pSessionInterface->GetSessionSettings( SESSION_NAME );
		sessionSettings.Set( CURRENT_PLAYERS_KEY, currentPlayers, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );
		m_pSessionInterface->UpdateSession( SESSION_NAME, sessionSettings );

		UpdateServerVisibility( sessionSettings.NumPublicConnections > currentPlayers );
	}
}

void UMyOnlineGameInstance::UpdateServerVisibility( bool visible )
{
	if( m_pSessionInterface.IsValid() )
	{
		FOnlineSessionSettings sessionSettings = *m_pSessionInterface->GetSessionSettings( SESSION_NAME );

		sessionSettings.bShouldAdvertise = visible;
		sessionSettings.bAllowJoinInProgress = visible;
		sessionSettings.bAllowJoinViaPresence = visible;

		m_pSessionInterface->UpdateSession( SESSION_NAME, sessionSettings );
	}
}

void UMyOnlineGameInstance::CreateSession( const FServerData& serverData )
{
	if( m_pSessionInterface.IsValid() )
	{
		FOnlineSessionSettings sessionSettings;
		if( IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" || serverData.isLAN )
		{
			sessionSettings.bIsLANMatch = true;
		}
		else
		{
			sessionSettings.bIsLANMatch = false;
		}
		// Set number of max players
		sessionSettings.NumPublicConnections = serverData.maxPlayers;

		// Set server visibility
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUsesPresence = true;


		sessionSettings.bAllowJoinInProgress = true;
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.bIsDedicated = false;

		// Set Game Mode ( passed as an int32 key value )
		const auto sessionGameMode = static_cast< int32 >( serverData.gameMode );
		// Set Current Players
		const int32 currentPlayers = 1;

		// Set server settings
		sessionSettings.Set( CURRENT_PLAYERS_KEY, currentPlayers, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );

		sessionSettings.Set( GAMEMODE_SESSION_KEY, sessionGameMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );

		sessionSettings.Set( SERVER_NAME_KEY, serverData.name, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing );

		m_pSessionInterface->CreateSession( 0, SESSION_NAME, sessionSettings );
	}
}

void UMyOnlineGameInstance::Join( const FString& Address )
{
	if( m_pMainMenu != nullptr )
	{
	}
	//UEngine* Engine = GetEngine();
	//if ( !ensure( Engine != nullptr ) )
	//{
	//	return;
	//}
	//Engine->AddOnScreenDebugMessage( 0, 5, FColor::Green, FString::Printf( TEXT( "Joining %s" ), *Address) );

	//APlayerController* PlayerController = GetFirstLocalPlayerController();
	//if ( !ensure( PlayerController != nullptr ) )
	//{
	//	return;
	//}
	//PlayerController->ClientTravel( FString::Printf( *Address ), ETravelType::TRAVEL_Absolute );
}

void UMyOnlineGameInstance::Join( const uint32 index )
{
	if( !m_pSessionInterface.IsValid() ) return;
	if( !m_pSessionSearch.IsValid() ) return;
	m_pSessionInterface->JoinSession( 0, SESSION_NAME, m_pSessionSearch->SearchResults[index] );
}

bool UMyOnlineGameInstance::IsRunningSteam()
{
	return IOnlineSubsystem::Get()->GetSubsystemName() == "STEAM";
}
