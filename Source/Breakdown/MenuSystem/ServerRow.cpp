// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"

#include "Breakdown/FServerData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MainMenu.h"

void UServerRow::Setup(const FServerData serverData, UMainMenu* pParent, uint32 index )
{
	// set server name
	ServerName->SetText(FText::FromString(serverData.name));

	// set host username
	HostUsername->SetText(FText::FromString(serverData.hostUsername));
	
	// set gamemode text with enum switch
	switch (serverData.gameMode)
	{
		case EBreakdownGameMode::FreeForAll:
		GameMode->SetText(FText::FromString(FString::Printf(TEXT("Free For All"))));
		break;
		case EBreakdownGameMode::TeamDeathMatch:
		GameMode->SetText(FText::FromString(FString::Printf(TEXT("TDM"))));
		break;
		default:
		GameMode->SetText(FText::FromString( FString::Printf(TEXT("UNDEFINED"))));
		break;
	}

	// set connection fraction (current/max)
	FString currentConectionsText = FString::Printf(TEXT("%d/%d"), serverData.currentPlayers, serverData.maxPlayers);
	CurrentConnections->SetText(FText::FromString(currentConectionsText));
	
	// set the rest of the data
	m_pParent = pParent;
	m_index = index;

	// add button functionality
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	SelectIndex();
}

void UServerRow::SelectIndex()
{
	m_pParent->SelectIndex( m_index, this );
}

void UServerRow::ToggleHighlight( bool highlighted )
{
	if( highlighted )
	{
		HighlightBackground->SetVisibility( ESlateVisibility::Visible );
	}
	else
	{
		HighlightBackground->SetVisibility( ESlateVisibility::Collapsed );
	}
}
