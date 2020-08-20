#pragma once

#include "Blueprint/UserWidget.h"
#include "Breakdown/EBreakdownGameMode.h"
#include "Breakdown/FServerData.h"
#include "CoreMinimal.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

class UButton;
class UComboBoxString;
class UEditableTextBox;
class UOverlay;
class UPanelWidget;
class UServerRow;
class UUserWidget;
class UWidget;
class UWidgetSwitcher;

UCLASS()
class BREAKDOWN_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
		
public:		
	UMainMenu( const FObjectInitializer& ObjectInitializer );
	void SetMenuInterface( IMenuInterface* MenuInterface );
	void Setup();

	void CreateServerList( TArray<FServerData> serverDatas );
	void SelectIndex( uint32 index, UServerRow* newHighlighted );
	void SelectIndex( uint32 index );

	const int GetSearchMode();

	UFUNCTION ( BlueprintCallable )
	UPanelWidget* GetServerList();

	UFUNCTION ( BlueprintCallable )
	UOverlay* GetFindingServersOverlay();

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld( ULevel* InLevel, UWorld* InWorld ) override;

	UPROPERTY( BlueprintReadWrite )
	bool isTDM = false;

	UPROPERTY( BlueprintReadWrite )
	int maxPlayers = 8;

	UFUNCTION( BlueprintImplementableEvent )
	void OnServerListCreated();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UEditableTextBox* ServerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY( BlueprintReadWrite )
	UServerRow* m_pHighlightedServerRow;

	UPROPERTY( BlueprintReadOnly, meta = ( BindWidget ) )
	UButton* ConnectToIPButton;




	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	TSubclassOf< UUserWidget > ServerRowClass;


	UFUNCTION( BlueprintCallable )
		void HostServer();

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
		void OpenJoinMenu();

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
		void OpenOnlineMenu();

	UFUNCTION( BlueprintCallable )
		void OpenHostMenu();
	
	
	
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
	void JoinServer();



	private:

	bool canConnect = false;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateSessionButton;
		
	UPROPERTY(meta = (BindWidget))
	UButton* BackToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;
		
	UPROPERTY(meta = (BindWidget))
	UWidget* OnlineMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ServerList;
			
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* OnlineModeComboString;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* SearchModeBox;

	UPROPERTY(meta = (BindWdiget))
	UOverlay* FindingServersOverlay;
		
	TOptional<uint32> m_selectedIndex;
	IMenuInterface* m_pMenuInterface;
};
