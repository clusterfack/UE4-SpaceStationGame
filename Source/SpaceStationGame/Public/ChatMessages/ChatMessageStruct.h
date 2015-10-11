#pragma once

#include "ChatMessageStruct.generated.h"

/* This is the struct for all chat messages, it should be only kept on the server. Don't replicate too much crap, this is going to be sent to all players on the server. */

USTRUCT()
struct FChatMessageStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString PlayerName;

	UPROPERTY()
		FString ChatMsg;

	UPROPERTY()
		FVector Location;

	UPROPERTY()
		uint8 Channel;

};


/* The client version of the struct*/
USTRUCT(BlueprintType)
struct FClientChatMessageStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default )
		FString ChatMsg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default )
		FColor MessageColor;
};