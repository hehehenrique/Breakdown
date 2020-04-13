// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"

#include "Components/Button.h"
#include "MainMenu.h"

void UServerRow::Setup( UMainMenu* pParent, uint32 index )
{
	m_pParent = pParent;
	m_index = index;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	m_pParent->SelectIndex( m_index );
}
