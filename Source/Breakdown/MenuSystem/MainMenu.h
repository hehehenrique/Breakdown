// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Breakdown/EBreakdownGameMode.h"
#include "Breakdown/FServerData.h"
#include "CoreMinimal.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

class UUserWidget;
class UButton;
class UWidgetSwitcher;
class UWidget;
class UPanelWidget;
class UEditableTextBox;



UCLASS()
class BREAKDOWN_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
		
public:		
	UMainMenu( const FObjectInitializer& ObjectInitializer );

	void SetMenuInterface( IMenuInterface* MenuInterface );
	void Setup();

	void CreateServerList( TArray<FServerData> serverDatas );
	void SelectIndex(uint32 index);

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld( ULevel* InLevel, UWorld* InWorld ) override;


private:
	TSubclassOf< UUserWidget > ServerRowClass;

		UPROPERTY(meta = (BindWidget))
		UButton* HostButton;

		UPROPERTY(meta = (BindWidget))
		UButton* JoinButton;

		UPROPERTY(meta = (BindWidget))
		UButton* ConnectToIPButton;

		UPROPERTY(meta = (BindWidget))
		UButton* CreateSessionButton;
		
		UPROPERTY(meta = (BindWidget))
		UButton* BackToMainMenuButton;

		UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* MenuSwitcher;

		UPROPERTY(meta = (BindWidget))
		UWidget* JoinMenu;
		
		UPROPERTY(meta = (BindWidget))
		UWidget* OnlineMenu;

		UPROPERTY(meta = (BindWidget))
		UWidget* HostMenu;

		UPROPERTY(meta = (BindWidget))
		UPanelWidget* ServerList;

		UPROPERTY(meta = (BindWidget))
		UEditableTextBox* MaxPlayersText;

		UPROPERTY(meta = (BindWidget))
		UEditableTextBox* ServerNameText;

		

		UFUNCTION(BlueprintCallable)
		void HostServer();
		
		UFUNCTION(BlueprintCallable)
		void JoinServer();

		UFUNCTION(BlueprintCallable)
		void OpenJoinMenu();

		UFUNCTION(BlueprintCallable)
		void OpenOnlineMenu();

		UFUNCTION(BlueprintCallable)
		void OpenHostMenu();

		TOptional<uint32> m_selectedIndex;
		IMenuInterface* m_pMenuInterface;
};
