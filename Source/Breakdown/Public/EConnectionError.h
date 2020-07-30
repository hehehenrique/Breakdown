#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"

UENUM( BlueprintType )
enum class EConnectionError: uint8
{
	SessionIsFull,	// Reached max players
	InvalidSession, // No longer exists
	Unknown			// That which is not known 
};
