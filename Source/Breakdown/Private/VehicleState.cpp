// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleState.h"

void AVehicleState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	ReadyForLobbyWidget();
}
