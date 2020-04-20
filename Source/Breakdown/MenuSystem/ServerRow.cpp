// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MainMenu.h"


void UServerRow::Setup(const FServerData serverData, UMainMenu* pParent, uint32 index )
{
	ServerName->SetText(FText::FromString(serverData.name));
	HostUsername->SetText(FText::FromString(serverData.hostUsername));
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

	FString currentConectionsText = FString::Printf(TEXT("%d/%d"), serverData.currentPlayers, serverData.maxPlayers);
	CurrentConnections->SetText(FText::FromString(currentConectionsText));
	
	m_pParent = pParent;
	m_index = index;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	m_pParent->SelectIndex( m_index );
}