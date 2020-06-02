// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VehicleController.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FGetSteamAvatarDelegate, const UTexture2D&, steamUserAvatar );



UCLASS()
class BREAKDOWN_API AVehicleController : public APlayerController
{
	GENERATED_BODY()

public:
	void TestFunction();



	
	//// Get a texture of a valid friends avatar, STEAM ONLY, Returns invalid texture if the subsystem hasn't loaded that size of avatar yet
	//UFUNCTION( BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI", meta = ( ExpandEnumAsExecs = "Result" ) )
	//	static UTexture2D * GetSteamFriendAvatar( const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize = SteamAvatarSize::SteamAvatar_Medium );

};
