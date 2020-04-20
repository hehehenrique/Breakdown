#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

struct FServerData;
class UButton;
class UMainMenu;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class BREAKDOWN_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(const FServerData serverData, UMainMenu* pParent, uint32 index);

protected:
	// Server Row Components
	UPROPERTY( meta = ( BindWidget ) )
	UButton* RowButton;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* GameMode;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* ServerName;
	
	UPROPERTY( BlueprintReadWrite, meta = ( BindWidget ) )
	UImage* HostAvatar;	
	
	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* HostUsername;

	UPROPERTY( meta = ( BindWidget ) )
	UTextBlock* CurrentConnections;
	
	UPROPERTY()
	UMainMenu* m_pParent;
	
	uint32 m_index;
	
	UFUNCTION()
	void OnClicked();
};