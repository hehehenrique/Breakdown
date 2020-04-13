// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "MenuInterface.h"

#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BREAKDOWN_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
			

		
public:		
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	void SetMenuInterface( IMenuInterface* MenuInterface );
	void Setup();

	void CreateServerList(TArray<FString> ServerNames);
	void SelectIndex(uint32 index);

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld( ULevel* InLevel, UWorld* InWorld ) override;


private:
	TSubclassOf< class UUserWidget > ServerRowClass;

		UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;

		UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;
		
		UPROPERTY(meta = (BindWidget))
		class UButton* ConnectToIPButton;
		
		UPROPERTY(meta = (BindWidget))
		class UButton* BackToMainMenuButton;

		UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

		UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;
		
		UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;		

		UPROPERTY(meta = (BindWidget))
		class UPanelWidget* ServerList;

		UFUNCTION()
		void HostServer();
		
		UFUNCTION()
		void JoinServer();

		UFUNCTION()
		void OpenJoinMenu();
		
		UFUNCTION()
		void OpenMainMenu();


		TOptional<uint32> m_selectedIndex;
		IMenuInterface* m_pMenuInterface;
};
