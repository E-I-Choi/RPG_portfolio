#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "GameTypes.h"
#include "Net/UnrealNetwork.h"

#include "RPGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharNameChanged, FName, NewName);


/**
* @brief 현재 사용하지 않음.
*/
UCLASS()
class RPG_PORTFOLIO_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ARPGPlayerState();

	
};
