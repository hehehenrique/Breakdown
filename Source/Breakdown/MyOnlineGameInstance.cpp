// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlineGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"

const static FName SESSION_NAME = TEXT("Henrique Session Game");

UMyOnlineGameInstance::UMyOnlineGameInstance(const FObjectInitializer& ObjectInitializer)
	: UGameInstance( ObjectInitializer )
	, desiredMap("/Game/Levels/Quarry_V6")
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

void UMyOnlineGameInstance::LoadMenu()
{
	if (!ensure(MainMenuClass != nullptr))
	{
		return;
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		return;
	}

	m_pMainMenu = CreateWidget < UMainMenu >(this, MainMenuClass);
	m_pMainMenu->bIsFocusable = true;

	m_pMainMenu->Setup();
	m_pMainMenu->SetMenuInterface(this);
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

void UMyOnlineGameInstance::OnCreateSessionComplete( FName sessionName, bool success )
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
	World->ServerTravel(desiredMap.Append("?listen"));

}

void UMyOnlineGameInstance::OnDestroySessionComplete( FName sessionName, bool success ) {
	if (success) {
		CreateSession();
	}
}

void UMyOnlineGameInstance::RefreshServerList()
{
	m_pSessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (m_pSessionSearch.IsValid())
	{
		//m_pSessionSearch->bIsLanQuery = true;
		m_pSessionSearch->MaxSearchResults = 100; // Need to set this to a high number so we can then filter out lobbies that are not Breakdown. This is because we are using the default steam dev app id 480
		m_pSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Started Finding Sessions"));

		m_pSessionInterface->FindSessions(0, m_pSessionSearch.ToSharedRef());
	}
}

void UMyOnlineGameInstance::OnFindSessionsComplete( bool success )
{
	if (success && m_pSessionSearch.IsValid() && m_pMainMenu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Finding Sessions."));
		TArray<FString> serverNames;
		for (const FOnlineSessionSearchResult& searchResult : m_pSessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *searchResult.GetSessionIdStr());
			serverNames.Add(searchResult.GetSessionIdStr());
		}
		m_pMainMenu->CreateServerList(serverNames);
	}
}

void UMyOnlineGameInstance::OnJoinSessionComplete( FName sessionName, EOnJoinSessionCompleteResult::Type result )
{
	if ( !m_pSessionInterface.IsValid() ) return;
	
	FString address;
	if ( !m_pSessionInterface->GetResolvedConnectString( sessionName, address ) ) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr))	return;
	
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();

	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(FString::Printf(*address), ETravelType::TRAVEL_Absolute);
}

void UMyOnlineGameInstance::CreateSession()
{
	if (m_pSessionInterface.IsValid())
	{
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bIsLANMatch = false;
		sessionSettings.NumPublicConnections = 2;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUsesPresence = true;

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

