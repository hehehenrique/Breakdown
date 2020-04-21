// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlineGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"

const static FName SESSION_NAME = TEXT("BCU Breakdown");
const static FName GAMEMODE_SESSION_KEY = TEXT("EBreakdownGameMode");
const static FName BCU_BREAKDOWN_AUTH = TEXT("isBCUBreakdown");

UMyOnlineGameInstance::UMyOnlineGameInstance(const FObjectInitializer& ObjectInitializer)
	: UGameInstance( ObjectInitializer )
	, desiredMap("/Game/Levels/Quarry_V6")
	, m_pMainMenu(nullptr)
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

void UMyOnlineGameInstance::Host()
{
	if (m_pSessionInterface.IsValid())
	{
		FNamedOnlineSession* existingSession = m_pSessionInterface->GetNamedSession(SESSION_NAME);
		if (existingSession != nullptr)
		{
			m_pSessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
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
		CreateSession();
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
		m_pSessionSearch->QuerySettings.Set(BCU_BREAKDOWN_AUTH, true, EOnlineComparisonOp::Equals);
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
		TArray<FString> serverNames;

		for (const FOnlineSessionSearchResult& searchResult : m_pSessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *searchResult.GetSessionIdStr());

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
			serverDatas.Add( serverData );
		}
		m_pMainMenu->CreateServerList(serverNames);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Finding Sessions BUUUUUUUUUT."));
	}
}

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

	PlayerController->ClientTravel(FString::Printf(*address), ETravelType::TRAVEL_Absolute);
}

void UMyOnlineGameInstance::CreateSession()
{
	if (m_pSessionInterface.IsValid())
	{
		FOnlineSessionSettings sessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			sessionSettings.bIsLANMatch = true;
		}
		else 
		{
			sessionSettings.bIsLANMatch = false;
		}

		sessionSettings.NumPublicConnections = 10;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUsesPresence = true;

		// Set Game Mode(passed as an int32 key value)
		auto sessionGameMode = static_cast< int32 >( EBreakdownGameMode::FreeForAll );
		
		sessionSettings.Set(GAMEMODE_SESSION_KEY, sessionGameMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		sessionSettings.Set(BCU_BREAKDOWN_AUTH, true, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		m_pSessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
	}
}

void UMyOnlineGameInstance::Join(const FString& Address)
{
	if (m_pMainMenu != nullptr)
	{
		m_pMainMenu->CreateServerList({ "heello i debug" });
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