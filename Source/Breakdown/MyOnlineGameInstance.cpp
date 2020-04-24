
#include "MyOnlineGameInstance.h"
//#include "ThirdParty/Steamworks/Steamv146/sdk/public/steam/isteamfriends.h"

//#include "ThirdParty/Steamworks/Steamv146/sdk/public/steam/steamclientpublic.h"
//#include "Source/Private/OnlineFriendsInterfaceSteam.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "EBreakdownGameMode.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"

const static FName SESSION_NAME = TEXT("BCU Breakdown");
const static FName GAMEMODE_SESSION_KEY = TEXT("EBreakdownGameMode");
const static FName BCU_BREAKDOWN_AUTH_KEY = TEXT("isBCUBreakdown");
const static FName SERVER_NAME = TEXT("ServerName");

UMyOnlineGameInstance::UMyOnlineGameInstance(const FObjectInitializer& ObjectInitializer)
	: UGameInstance( ObjectInitializer )
	, desiredMap("/Game/Levels/Quarry_V6")
	, m_pMainMenu( nullptr )
{
	ConstructorHelpers::FClassFinder<UMainMenu> MainMenuWBPClass(TEXT("/Game/MenuSystem/WBP_OnlineMainMenu"));

	if (!ensure(MainMenuWBPClass.Class != nullptr))
	{
		return;
	}
	MainMenuClass = MainMenuWBPClass.Class;
}

void UMyOnlineGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MainMenuClass->GetName());
	IOnlineSubsystem* pOnlineSubsystem = IOnlineSubsystem::Get();
	if (pOnlineSubsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found online subsystem %s"), *pOnlineSubsystem->GetSubsystemName().ToString());
		m_pSessionInterface = pOnlineSubsystem->GetSessionInterface();
		if (m_pSessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session interface"));
			m_pSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyOnlineGameInstance::OnCreateSessionComplete);
			m_pSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyOnlineGameInstance::OnDestroySessionComplete);
			m_pSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyOnlineGameInstance::OnFindSessionsComplete);
			m_pSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyOnlineGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No online subsystem is configured"));
	}
}

UMainMenu* UMyOnlineGameInstance::LoadOnlineMenu()
{
	if (m_pMainMenu == nullptr) {
	if (!ensure(MainMenuClass != nullptr))
		{
			return nullptr;
		}

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (!ensure(PlayerController != nullptr))
		{
			return nullptr;
		}

		m_pMainMenu = CreateWidget < UMainMenu >(this, MainMenuClass);
		m_pMainMenu->bIsFocusable = true;

		m_pMainMenu->Setup();
		m_pMainMenu->SetMenuInterface(this);
	}
	return m_pMainMenu;
}

void UMyOnlineGameInstance::Host(const FServerData& serverData)
{
	m_hostingServerData = serverData;
	if (m_pSessionInterface.IsValid())
	{
		FNamedOnlineSession* existingSession = m_pSessionInterface->GetNamedSession(SESSION_NAME);
		if (existingSession != nullptr)
		{
			m_pSessionInterface->DestroySession(SESSION_NAME);	
		}
		else
		{
			CreateSession(serverData);
		}
	}
}

void UMyOnlineGameInstance::OnCreateSessionComplete_Implementation( FName sessionName, bool success )
{
	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session."));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		return;
	}
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		return;
	}
	World->ServerTravel("/Game/Levels/Quarry_V6?listen");
}

void UMyOnlineGameInstance::OnDestroySessionComplete_Implementation( FName sessionName, bool success ) {
	if (success) 
	{
		CreateSession(m_hostingServerData);
	}
}

void UMyOnlineGameInstance::RefreshServerList()
{
	m_pSessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (m_pSessionSearch.IsValid())
	{
		//m_pSessionSearch->bIsLanQuery = true;
		m_pSessionSearch->MaxSearchResults = 200; // Need to set this to a high number so we can then filter out lobbies that are not Breakdown. This is because we are using the default steam dev app id 480
		m_pSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		m_pSessionSearch->QuerySettings.Set(BCU_BREAKDOWN_AUTH_KEY, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Started Finding Sessions"));

		m_pSessionInterface->FindSessions(0, m_pSessionSearch.ToSharedRef());
	}
}

void UMyOnlineGameInstance::OnFindSessionsComplete_Implementation( bool success )
{
	if (success && m_pSessionSearch.IsValid() && m_pMainMenu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Finding Sessions.!!!"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), m_pSessionSearch->SearchResults.Num());
		
		
		// Array of FServerData that we will use to create our server list
		TArray<FServerData> serverDatas;

		// For loop to configure each serverData and populate the array
		for (const FOnlineSessionSearchResult& searchResult : m_pSessionSearch->SearchResults)
		{
			// Bool to make sure this is a valid server, and should be added into the list
			bool isValidServer = true;

			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *searchResult.GetSessionIdStr());

			// Create a new serverData struct
			FServerData serverData;

			// Set struct variables
			serverData.name = searchResult.GetSessionIdStr();
			serverData.maxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			serverData.currentPlayers = serverData.maxPlayers - searchResult.Session.NumOpenPublicConnections;
			serverData.hostUsername = searchResult.Session.OwningUserName;

			TSharedPtr < const FUniqueNetId > hostUserID = searchResult.Session.OwningUserId;
		
			//CSteamID hostSteamID ( dynamic_cast<uint64>( *hostUserID->ToString() ) );
			UE_LOG(LogTemp, Warning, TEXT("Host ID: %s"), *hostUserID->ToString());
			if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
			{
				int32 steamID = (FCString::Atoi(*hostUserID->ToString()));
				UE_LOG(LogTemp, Warning, TEXT("Steam ID: %d"), steamID);
			}

			// Get the GameMode key and set the relevant struct var
			const FOnlineSessionSetting* pGameModeKey = ( searchResult.Session.SessionSettings.Settings.Find(GAMEMODE_SESSION_KEY));
			
			// If found key
			if(pGameModeKey)
			{
				int32 serverGameMode;
				pGameModeKey->Data.GetValue( serverGameMode );
				serverData.gameMode = static_cast<EBreakdownGameMode>( serverGameMode );
				UE_LOG( LogTemp, Warning, TEXT("Found server game mode: %d"), serverData.gameMode );
			}
			else 
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not find gameMode."));
				isValidServer = false;
			}
			
			// Get the BCU_BREAKDOWN_AUTH_KEY, so we know if this server is for our game
			const FOnlineSessionSetting* pAuthenticServerKey = (searchResult.Session.SessionSettings.Settings.Find(BCU_BREAKDOWN_AUTH_KEY));
			// If found key
			if ( pAuthenticServerKey )
			{
				bool hasAuthenticKey;
				pAuthenticServerKey->Data.GetValue(hasAuthenticKey);
				if ( hasAuthenticKey )
				{
					UE_LOG(LogTemp, Warning, TEXT("This server has an authentic key."));
				}
				else 
				{
					isValidServer = false;
					UE_LOG(LogTemp, Warning, TEXT("This server has an authentic key, but is set to false."));
				}
			}
			else // If not, this is not a valid server for our game
			{
				isValidServer = false;
				UE_LOG(LogTemp, Warning, TEXT("This server does not have the authentic key."));
			}
				


			// Get the ServerName key and set the relevant struct var
			const FOnlineSessionSetting* pServerNameKey = (searchResult.Session.SessionSettings.Settings.Find(SERVER_NAME));

			// If found key
			if (pServerNameKey)
			{
				FString serverName;
				pServerNameKey->Data.GetValue(serverName);
				serverData.name = serverName;
				UE_LOG(LogTemp, Warning, TEXT("Found server name: %s"), *serverName);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not find server name."));
				isValidServer = false;
			}



			// Finally, if server is valid, insert in server list
			if( isValidServer )
			{
				serverDatas.Add( serverData );
			}
		}
		m_pMainMenu->CreateServerList( serverDatas );
	}
	else 
	{
		UE_LOG( LogTemp, Warning, TEXT( "Finished Finding Sessions BUUUUUUUUUT." ) );
	}
}

//
//int iImage = SteamFriends()->GetMediumFriendAvatar(playerSteamID);
//HGAMETEXTURE hTexture = 0;
//if (iImage != -1)
//hTexture = GetSteamImageAsTexture(iImage);
//
//RECT rect;
//switch (i)
//{
//case 0:
//	rect.top = nHudPaddingVertical;
//	rect.bottom = rect.top + nAvatarHeight;
//	rect.left = nHudPaddingHorizontal;
//	rect.right = rect.left + scorewidth;
//
//	if (hTexture)
//	{
//		m_pGameEngine->BDrawTexturedRect((float)rect.left, (float)rect.top, (float)rect.left + nAvatarWidth, (float)rect.bottom,
//			0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB(255, 255, 255, 255), hTexture);
//		rect.left += nAvatarWidth + nSpaceBetweenAvatarAndScore;
//		rect.right += nAvatarWidth + nSpaceBetweenAvatarAndScore;
//	}
//
//	sprintf_safe(rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState);
//	m_pGameEngine->BDrawString(m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_LEFT | TEXTPOS_VCENTER, rgchBuffer);
//	break;
//case 1:
//
//	rect.top = nHudPaddingVertical;
//	rect.bottom = rect.top + nAvatarHeight;
//	rect.left = width - nHudPaddingHorizontal - scorewidth;
//	rect.right = width - nHudPaddingHorizontal;
//
//	if (hTexture)
//	{
//		m_pGameEngine->BDrawTexturedRect((float)rect.right - nAvatarWidth, (float)rect.top, (float)rect.right, (float)rect.bottom,
//			0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB(255, 255, 255, 255), hTexture);
//		rect.right -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
//		rect.left -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
//	}
//
//	sprintf_safe(rgchBuffer, "%s\nScore: %2u ", rgchPlayerName, m_rguPlayerScores[i]);
//	m_pGameEngine->BDrawString(m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_RIGHT | TEXTPOS_VCENTER, rgchBuffer);
//	break;
//case 2:
//	rect.top = height - nHudPaddingVertical - nAvatarHeight;
//	rect.bottom = rect.top + nAvatarHeight;
//	rect.left = nHudPaddingHorizontal;
//	rect.right = rect.left + scorewidth;
//
//	if (hTexture)
//	{
//		m_pGameEngine->BDrawTexturedRect((float)rect.left, (float)rect.top, (float)rect.left + nAvatarWidth, (float)rect.bottom,
//			0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB(255, 255, 255, 255), hTexture);
//		rect.right += nAvatarWidth + nSpaceBetweenAvatarAndScore;
//		rect.left += nAvatarWidth + nSpaceBetweenAvatarAndScore;
//	}
//
//	sprintf_safe(rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState);
//	m_pGameEngine->BDrawString(m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_LEFT | TEXTPOS_BOTTOM, rgchBuffer);
//	break;
//case 3:
//	rect.top = height - nHudPaddingVertical - nAvatarHeight;
//	rect.bottom = rect.top + nAvatarHeight;
//	rect.left = width - nHudPaddingHorizontal - scorewidth;
//	rect.right = width - nHudPaddingHorizontal;
//
//	if (hTexture)
//	{
//		m_pGameEngine->BDrawTexturedRect((float)rect.right - nAvatarWidth, (float)rect.top, (float)rect.right, (float)rect.bottom,
//			0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB(255, 255, 255, 255), hTexture);
//		rect.right -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
//		rect.left -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
//	}
//
//	sprintf_safe(rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState);
//	m_pGameEngine->BDrawString(m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_RIGHT | TEXTPOS_BOTTOM, rgchBuffer);
//	break;
//default:
//	OutputDebugString("DrawHUDText() needs updating for more players\n");
//	break;
//}
//	}
//
//	// Draw a Steam Input tooltip
//	if (m_pGameEngine->BIsSteamInputDeviceActive())
//	{
//		char rgchHint[128];
//		const char* rgchFireOrigin = m_pGameEngine->GetTextStringForControllerOriginDigital(eControllerActionSet_ShipControls, eControllerDigitalAction_FireLasers);
//
//		if (strcmp(rgchFireOrigin, "None") == 0)
//		{
//			sprintf_safe(rgchHint, "No Fire action bound.");
//		}
//		else
//		{
//			sprintf_safe(rgchHint, "Press '%s' to Fire", rgchFireOrigin);
//		}
//
//		RECT rect;
//		int nBorder = 30;
//		rect.top = m_pGameEngine->GetViewportHeight() - nBorder;
//		rect.bottom = m_pGameEngine->GetViewportHeight() * 2;
//		rect.left = nBorder;
//		rect.right = m_pGameEngine->GetViewportWidth();
//		m_pGameEngine->BDrawString(m_hHUDFont, rect, D3DCOLOR_ARGB(255, 255, 255, 255), TEXTPOS_LEFT | TEXTPOS_TOP, rgchHint);
//	}



void UMyOnlineGameInstance::OnJoinSessionComplete( FName sessionName, EOnJoinSessionCompleteResult::Type result )
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete called"));
	if ( !m_pSessionInterface.IsValid() ) return;
	
	FString address;
	if ( !m_pSessionInterface->GetResolvedConnectString( sessionName, address ) ) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr))	return;
	
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	if (!ensure(PlayerController != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("OnJoinSessionComplete called. Joining address %s"), *address));

	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete called. ClientTravel will be called in next line. Joining address %s"), *address);

	PlayerController->ClientTravel((address), ETravelType::TRAVEL_Absolute);
}

void UMyOnlineGameInstance::CreateSession( const FServerData& serverData )
{
	if ( m_pSessionInterface.IsValid() )
	{
		FOnlineSessionSettings sessionSettings;
		if ( IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" )
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

		// Set Game Mode(passed as an int32 key value)
		auto sessionGameMode = static_cast< int32 >( serverData.gameMode );
		
		sessionSettings.Set(GAMEMODE_SESSION_KEY, sessionGameMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		sessionSettings.Set(BCU_BREAKDOWN_AUTH_KEY, true, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		sessionSettings.Set(SERVER_NAME, serverData.name, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		m_pSessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
	}
}

void UMyOnlineGameInstance::Join(const FString& Address)
{
	if (m_pMainMenu != nullptr)
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

void UMyOnlineGameInstance::Join(const uint32 index)
{
	if (!m_pSessionInterface.IsValid()) return;
	if (!m_pSessionSearch.IsValid()) return;
	m_pSessionInterface->JoinSession(0, SESSION_NAME, m_pSessionSearch->SearchResults[index]);

	if (m_pMainMenu != nullptr)
	{
	}
}