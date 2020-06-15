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

	UFUNCTION(BlueprintCallable)
	void TravelToDesiredLevel();

protected:
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int m_playerCount = 0;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString m_desiredMap;
};
