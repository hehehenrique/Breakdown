// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "ServerRow.h"
#include "UObject/ConstructorHelpers.h"

UMainMenu::UMainMenu( const FObjectInitializer& ObjectInitializer )
	: UUserWidget( ObjectInitializer )
{
	ConstructorHelpers::FClassFinder< UUserWidget > ServerRowWBPClass( TEXT( "/Game/MenuSystem/WBP_ServerRow" ) );

	if ( !ensure( ServerRowWBPClass.Class != nullptr ) )
	{
		return;
	}
	ServerRowClass = ServerRowWBPClass.Class;
}

void UMainMenu::Setup() 
{
	this->AddToViewport();

	UWorld* pWorld = GetWorld();
	if ( !ensure( pWorld != nullptr ) ) return;

	APlayerController* pPlayerCon = pWorld->GetFirstPlayerController();
	if ( !ensure( pPlayerCon != nullptr ) ) return;


	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus( this->TakeWidget() );
	InputModeData.SetLockMouseToViewportBehavior( EMouseLockMode::DoNotLock );

	pPlayerCon->SetInputMode(InputModeData);
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
		
		HostButton->OnClicked.AddDynamic( this, &UMainMenu::HostServer );
		JoinButton->OnClicked.AddDynamic( this, &UMainMenu::OpenJoinMenu );
		ConnectToIPButton->OnClicked.AddDynamic( this, &UMainMenu::JoinServer );
		BackToMainMenuButton->OnClicked.AddDynamic( this, &UMainMenu::OpenMainMenu );
		return true;
	}
	else return false;
}

void UMainMenu::HostServer() 
{
	if ( m_pMenuInterface != nullptr )
	{
		m_pMenuInterface->Host();
	}
	else 
	{
		UE_LOG( LogTemp, Warning, TEXT( "MENU INTERFACE POINTER IS NULLPTR" ) );
	}
}

void UMainMenu::CreateServerList(TArray<FString> ServerNames)
{

	ServerList->ClearChildren();
	uint32 serverRowIndex = 0;
	for ( const FString& serverName : ServerNames ) 
	{
		UServerRow* serverRow = CreateWidget < UServerRow >( this, ServerRowClass );
		if ( !ensure( serverRow != nullptr ) ) 
		{
			return;
		}
		serverRow->ServerName->SetText( FText::FromString( serverName ) );
		serverRow->Setup( this, serverRowIndex );
		++serverRowIndex;
		ServerList->AddChild( serverRow );
	}
}

void UMainMenu::SelectIndex( uint32 index )
{
	m_selectedIndex = index;
}

void UMainMenu::JoinServer() 
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
	if ( m_pMenuInterface != nullptr ) {
		m_pMenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenMainMenu() 
{
	if ( !ensure( MenuSwitcher != nullptr ) ) return;
	if ( !ensure( MainMenu != nullptr ) ) return;
	MenuSwitcher->SetActiveWidget( MainMenu );
}