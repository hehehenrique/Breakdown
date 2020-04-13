// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

class UMainMenu;

/**
 * 
 */
UCLASS()
class BREAKDOWN_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	void Setup(UMainMenu* pParent, uint32 index);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;
	
	UPROPERTY()
	class UMainMenu* m_pParent;
	
	uint32 m_index;
	
	UFUNCTION()
	void OnClicked();
};
