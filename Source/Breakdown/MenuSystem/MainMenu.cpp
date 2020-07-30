// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Breakdown/FServerData.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Misc/DefaultValueHelper.h"
#include "ServerRow.h"
#include "UObject/ConstructorHelpers.h"

UMainMenu::UMainMenu( const FObjectInitializer& ObjectInitializer )
	: UUserWidget( ObjectInitializer )
{
	//ConstructorHelpers::FClassFinder< UUserWidget > ServerRowWBPClass( TEXT( "/Game/MenuSystem/WBP_ServerRow" ) );

	//if ( !ensure( ServerRowWBPClass.Class != nullptr ) )
	//{
	//	return;
	//}
	//ServerRowClass = ServerRowWBPClass.Class;
}

void UMainMenu::Setup() 
{
	MenuSwitcher->SetActiveWidget( OnlineMenu );
	this->AddToViewport();

	UWorld* pWorld = GetWorld();
	if ( !ensure( pWorld != nullptr ) ) return;

	APlayerController* pPlayerCon = pWorld->GetFirstPlayerController();
	if ( !ensure( pPlayerCon != nullptr ) ) return;


	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus( this->TakeWidget() );
	InputModeData.SetLockMouseToViewportBehavior( EMouseLockMode::DoNotLock );

	pPlayerCon->SetInputMode( InputModeData );
	pPlayerCon->bShowMouseCursor = true;
}

void UMainMenu::OnLevelRemovedFromWorld( ULevel* InLevel, UWorld* InWorld ) 
{
	UE_LOG(LogTemp, Warning, TEXT("OnLevelRemovedFromWorld"));

	this->RemoveFromViewport();

	UWorld* pWorld = GetWorld();
	if ( !ensure( pWorld != nullptr ) ) return;

	APlayerController* pPlayerCon = pWorld->GetFirstPlayerController();
	if ( !ensure( pPlayerCon != nullptr ) ) return;
	 

	FInputModeGameOnly InputModeData;

	pPlayerCon->SetInputMode( InputModeData );
	pPlayerCon->bShowMouseCursor = false;
}

void UMainMenu::SetMenuInterface(IMenuInterface* MenuInterface) 
{
	m_pMenuInterface = MenuInterface;
}

bool UMainMenu::Initialize()
{
	if ( Super::Initialize() ) 
	{
		
		if ( !ensure( HostButton != nullptr ) ) return false;
		if ( !ensure( JoinButton != nullptr ) ) return false;
		if ( !ensure( BackToMainMenuButton != nullptr ) ) return false;
		if ( !ensure( ConnectToIPButton != nullptr ) ) return false;
		if ( !ensure( CreateSessionButton != nullptr ) ) return false;
		
		// Main Online Menu
		HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
		JoinButton->OnClicked.AddDynamic( this, &UMainMenu::OpenJoinMenu );
		// Join Servers Menu
		ConnectToIPButton->OnClicked.AddDynamic( this, &UMainMenu::JoinServer );
		BackToMainMenuButton->OnClicked.AddDynamic( this, &UMainMenu::OpenOnlineMenu );
		// Host Server Menu
		CreateSessionButton->OnClicked.AddDynamic( this, &UMainMenu::HostServer );
	
		return true;
	}
	else return false;
}

void UMainMenu::HostServer() 
{
	if ( m_pMenuInterface != nullptr )
	{
		
		FServerData serverData;
		
		FDefaultValueHelper::ParseInt(MaxPlayersText->Text.ToString(), serverData.maxPlayers);
		UE_LOG(LogTemp, Warning, TEXT("THE NUMBER OF MAX PLAYERS IS %d"), serverData.maxPlayers);

		// Set Server Name
		serverData.name = ServerNameText->Text.ToString();
		
		// Set Server GameMode
		switch (GameModeComboString->GetSelectedIndex())
		{
		case 0:
		{
			serverData.gameMode = EBreakdownGameMode::FreeForAll;
		}
		break;
		case 1:
		{
			serverData.gameMode = EBreakdownGameMode::TeamDeathMatch;
		}
		break;
		case 2:
		{
			serverData.gameMode = EBreakdownGameMode::BattleRoyale;
		}
		break;
		default:
		{
			serverData.gameMode = EBreakdownGameMode::FreeForAll;
		}
		break;
		}

		// Set Server Online Mode ( Steam / LAN )
		serverData.isLAN = OnlineModeComboString->GetSelectedIndex() == 1;

		m_pMenuInterface->Host( serverData );
	}
	else 
	{
		UE_LOG( LogTemp, Warning, TEXT( "MENU INTERFACE POINTER IS NULLPTR" ) );
	}
}

void UMainMenu::CreateServerList(TArray<FServerData> serverDatas )
{

	ServerList->ClearChildren();
	uint32 serverRowIndex = 0;
	for ( const FServerData& serverData : serverDatas )
	{
		UServerRow* serverRow = CreateWidget < UServerRow >( this, ServerRowClass );
		if ( !ensure( serverRow != nullptr ) ) 
		{
			return;
		}
		serverRow->Setup( serverData, this, serverRowIndex );
		++serverRowIndex;
		ServerList->AddChild( serverRow );
	}
	// If array of servers isn't empty
	if( serverDatas.Num() )
	{
		// Automatically select first server
		SelectIndex( 0 );
	}
	// If no servers are found
	else
	{
		// Disable connect button
		ConnectToIPButton->SetIsEnabled( false );
	}
}

void UMainMenu::SelectIndex( uint32 index, UServerRow* newHighlighted )
{
	if( m_pHighlightedServerRow != nullptr )
	{
		m_pHighlightedServerRow->ToggleHighlight( false );
	}
	m_pHighlightedServerRow = newHighlighted;
	m_pHighlightedServerRow->ToggleHighlight( true );

	m_selectedIndex = index;

	ConnectToIPButton->SetIsEnabled( true );
}

void UMainMenu::SelectIndex( uint32 index )
{
	if( m_pHighlightedServerRow != nullptr )
	{
		m_pHighlightedServerRow->ToggleHighlight( false );
	}
	// Get Server Row in the Server List to highlight
	auto newHighlighted = Cast<UServerRow> (ServerList->GetChildAt( index ));
	if( newHighlighted != nullptr )
	{
		m_pHighlightedServerRow = newHighlighted;
		m_pHighlightedServerRow->ToggleHighlight( true );
	}

	m_selectedIndex = index;

	ConnectToIPButton->SetIsEnabled( true );
}



void UMainMenu::JoinServer_Implementation() 
{
	if ( m_selectedIndex.IsSet() && m_pMenuInterface != nullptr )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Selected index %d" ), m_selectedIndex.GetValue() );
		m_pMenuInterface->Join( m_selectedIndex.GetValue() );
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "Selected index %d" ), m_selectedIndex.GetValue() );
	}
}

void UMainMenu::OpenJoinMenu() 
{
	if ( !ensure( MenuSwitcher != nullptr ) ) return;
	if ( !ensure( JoinMenu != nullptr ) ) return;
	MenuSwitcher->SetActiveWidget( JoinMenu );
	ConnectToIPButton->SetIsEnabled( false );
	if ( m_pMenuInterface != nullptr ) 
	{
		m_pMenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenOnlineMenu() 
{
	if ( !ensure( MenuSwitcher != nullptr ) ) return;
	if ( !ensure( OnlineMenu != nullptr ) ) return;
	MenuSwitcher->SetActiveWidget(OnlineMenu);
}

void UMainMenu::OpenHostMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(HostMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

const int UMainMenu::GetSearchMode()
{	
	return SearchModeBox->GetSelectedIndex();
}

UPanelWidget* UMainMenu::GetServerList()
{
	return ServerList;
}

UOverlay* UMainMenu::GetFindingServersOverlay()
{
	return FindingServersOverlay;
}
