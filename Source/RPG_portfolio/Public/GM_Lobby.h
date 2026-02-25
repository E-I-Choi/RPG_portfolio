
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PORTFOLIO_API AGM_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGM_Lobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
