
#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "NetworkTask.h"
#include "PC_Lobby.h"
#include "PC_RPG.h"
#include "GM_RPG.h"

#include "Core/PlayFabEconomyDataModels.h" 
#include "Core/PlayFabError.h"
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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
