

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GameTypes.h"

#include "NetworkResponseInterface.generated.h"


UINTERFACE(MinimalAPI)
class UNetworkResponseInterface : public UInterface
{
	GENERATED_BODY()
};


class RPG_PORTFOLIO_API INetworkResponseInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Network")
	void ReceiveNetResponse(const FNetworkReturnResult& Result);
	
};
