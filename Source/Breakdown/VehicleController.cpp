#include "VehicleController.h"

#include "Steamworks/Steamv146/sdk/public/steam/isteamfriends.h"
#include "Steamworks/Steamv146/sdk/public/steam/steam_api.h"
#include "OnlineSubsystemNames.h"

void AVehicleController::TestFunction()
{
	CSteamID steamID = SteamUser()->GetSteamID();
}



//UTexture2D * AVehicleController::GetSteamFriendAvatar( const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize )
//{
//#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
//	if( !UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM )
//	{
//		Result = EBlueprintAsyncResultSwitch::OnFailure;
//		return nullptr;
//	}
//
//	uint32 Width = 0;
//	uint32 Height = 0;
//
//	if( SteamAPI_Init() )
//	{
//		//Getting the PictureID from the SteamAPI and getting the Size with the ID
//		//virtual bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly ) = 0;
//
//
//		uint64 id = *( ( uint64* )UniqueNetId.UniqueNetId->GetBytes() );
//		int Picture = 0;
//
//		switch( AvatarSize )
//		{
//		case SteamAvatarSize::SteamAvatar_Small: Picture = SteamFriends()->GetSmallFriendAvatar( id ); break;
//		case SteamAvatarSize::SteamAvatar_Medium: Picture = SteamFriends()->GetMediumFriendAvatar( id ); break;
//		case SteamAvatarSize::SteamAvatar_Large: Picture = SteamFriends()->GetLargeFriendAvatar( id ); break;
//		default: break;
//		}
//
//		if( Picture == -1 )
//		{
//			Result = EBlueprintAsyncResultSwitch::AsyncLoading;
//			return NULL;
//		}
//
//		SteamUtils()->GetImageSize( Picture, &Width, &Height );
//
//		// STOLEN FROM ANSWERHUB :p, then fixed because answerhub wasn't releasing the memory O.o
//		// Also fixed image pixel format and switched to a memcpy instead of manual iteration.
//		// At some point I should probably reply to that answerhub post with these fixes to prevent people killing their games.....
//
//		if( Width > 0 && Height > 0 )
//		{
//			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
//			uint8 *oAvatarRGBA = new uint8[Width * Height * 4];
//
//
//			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
//			SteamUtils()->GetImageRGBA( Picture, ( uint8* )oAvatarRGBA, 4 * Height * Width * sizeof( char ) );
//
//
//			// Removed as I changed the image bit code to be RGB, I think the original author was unaware that there were different pixel formats
//			/*
//			//Swap R and B channels because for some reason the games whack
//			for (uint32 i = 0; i < (Width * Height * 4); i += 4)
//			{
//				uint8 Temp = oAvatarRGBA[i + 0];
//				oAvatarRGBA[i + 0] = oAvatarRGBA[i + 2];
//				oAvatarRGBA[i + 2] = Temp;
//			}*/
//
//			UTexture2D* Avatar = UTexture2D::CreateTransient( Width, Height, PF_R8G8B8A8 );
//			// Switched to a Memcpy instead of byte by byte transer
//			uint8* MipData = ( uint8* )Avatar->PlatformData->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
//			FMemory::Memcpy( MipData, ( void* )oAvatarRGBA, Height * Width * 4 );
//			Avatar->PlatformData->Mips[0].BulkData.Unlock();
//
//			// Original implementation was missing this!!
//			// the hell man......
//			delete[] oAvatarRGBA;
//
//			//Setting some Parameters for the Texture and finally returning it
//			Avatar->PlatformData->SetNumSlices( 1 );
//			Avatar->NeverStream = true;
//			//Avatar->CompressionSettings = TC_EditorIcon;
//
//			Avatar->UpdateResource();
//
//			Result = EBlueprintAsyncResultSwitch::OnSuccess;
//			return Avatar;
//		}
//		else
//		{
//			UE_LOG( AdvancedSteamFriendsLog, Warning, TEXT( "Bad Height / Width with steam avatar!" ) );
//		}
//
//		Result = EBlueprintAsyncResultSwitch::OnFailure;
//		return nullptr;
//	}
//#endif
//
//	UE_LOG( AdvancedSteamFriendsLog, Warning, TEXT( "STEAM Couldn't be verified as initialized" ) );
//	Result = EBlueprintAsyncResultSwitch::OnFailure;
//	return nullptr;
//}