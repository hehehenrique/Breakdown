// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	void SetDesiredMap( const FString& desiredMap );
private:
	uint32 m_playerCount = 0;
	FString m_desiredMap;
};
