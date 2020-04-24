// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "EBreakdownGameMode.h"
#include "UObject/Interface.h"
#include "FServerData.generated.h"
USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString name;
	int		currentPlayers;
	int		maxPlayers;
	FString hostUsername;
	EBreakdownGameMode gameMode;
	bool	isLAN;
};
