// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "MyOnlineGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API UMyOnlineGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:

	UMyOnlineGameInstance( const FObjectInitializer & ObjectInitializer );

	virtual void Init();

	UFUNCTION( BlueprintCallable )
	void LoadMenu();

	UFUNCTION(Exec)
	virtual void Host() override;
	
	UFUNCTION(Exec)
	virtual void Join( const FString& address ) override;
	virtual void Join( const uint32 index ) override;
	
	virtual void RefreshServerList() override;




private:


	TSubclassOf< class UUserWidget > MainMenuClass;
	class UMainMenu* m_pMainMenu;
	IOnlineSessionPtr m_pSessionInterface;
	TSharedPtr< class FOnlineSessionSearch > m_pSessionSearch;

	// Callbacks
	void OnCreateSessionComplete( FName sessionName, bool success );
	void OnDestroySessionComplete( FName sessionName, bool success );
	void OnFindSessionsComplete( bool success );
	void OnJoinSessionComplete( FName sessionName, EOnJoinSessionCompleteResult::Type result );

	void CreateSession();

protected:

	// Map directory to be loaded
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString desiredMap;
};
