// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EConnectionError.h"
#include "Engine/GameInstance.h"
#include "FServerData.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "MyOnlineGameInstance.generated.h"

class UMainMenu;

UCLASS()
class BREAKDOWN_API UMyOnlineGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:

	UMyOnlineGameInstance( const FObjectInitializer& ObjectInitializer );

	virtual void Init();

	UFUNCTION( BlueprintCallable )
		bool IsRunningSteam();

	UFUNCTION( BlueprintCallable )
		UMainMenu* LoadOnlineMenu();

	UFUNCTION( Exec )
		virtual void Host( const FServerData& serverData ) override;

	UFUNCTION( Exec )
		virtual void Join( const FString& address ) override;
	virtual void Join( const uint32 index ) override;

	UFUNCTION( BlueprintCallable )
		virtual void RefreshServerList() override;

	UFUNCTION( BlueprintCallable )
		void DestroySession();

private:

	IOnlineSessionPtr m_pSessionInterface;
	TSharedPtr< class FOnlineSessionSearch > m_pSessionSearch;

	// If we need to destroy the session before hosting
	FServerData m_hostingServerData;

	void CreateSession( const FServerData& serverData );

protected:
	
	UPROPERTY( EditAnywhere )
	TSubclassOf< class UUserWidget > MainMenuClass;

	// Map directory to be loaded
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString desiredMap;

	// Reference to the online menu widget
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UMainMenu* m_pMainMenu;

	// Callbacks
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, Category = "Online" )
		void OnCreateSessionComplete( FName sessionName, bool success );

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, Category = "Online" )
		void OnDestroySessionComplete( FName sessionName, bool success );

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, Category = "Online" )
		void OnFindSessionsComplete( bool success );

	void OnJoinSessionComplete( FName sessionName, EOnJoinSessionCompleteResult::Type result );

	// Runs when an error occurs after trying to join an existing session
	UFUNCTION( BlueprintImplementableEvent, Category = "Online | Connecting" )
		void ConnectionAttemptFailed( EConnectionError reason );
	
};