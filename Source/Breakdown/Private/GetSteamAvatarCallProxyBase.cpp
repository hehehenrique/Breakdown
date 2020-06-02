#include "GetSteamAvatarCallProxyBase.h"

UGetSteamAvatarCallProxyBase* UGetSteamAvatarCallProxyBase::GetSteamAvatar(UObject* InWorldContextObject,
	APlayerController* PlayerController)
{
	UGetSteamAvatarCallProxyBase* Proxy = NewObject<UGetSteamAvatarCallProxyBase>();
	Proxy->WorldContextObject = InWorldContextObject;
	Proxy->PlayerControllerWeakPtr = PlayerController;

	return Proxy;
}

void UGetSteamAvatarCallProxyBase::Activate()
{
}
