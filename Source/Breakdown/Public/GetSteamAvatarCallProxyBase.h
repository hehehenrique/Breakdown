#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "GetSteamAvatarCallProxyBase.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FGetSteamAvatarDelegate, const UTexture2D&, steamUserAvatar );


UCLASS()
class BREAKDOWN_API UGetSteamAvatarCallProxyBase : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

private:
	
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	UObject* WorldContextObject;
	
public:

	//UPROPERTY( BlueprintAssignable )
	//	FGetSteamAvatarDelegate OnSuccess;
	//
	//UPROPERTY( BlueprintAssignable )
	//	FGetSteamAvatarDelegate OnFailure;


	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject" ), Category = "Steam" )
		static UGetSteamAvatarCallProxyBase* GetSteamAvatar( UObject* InWorldContextObject, class APlayerController* PlayerController );
	
		virtual void Activate() override;
};
