#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "PC_Lobby.h"
#include "C:\EunIl\Unreal\Projects\C++\MultiPlay\RPG_portfolio\Source\RPG_portfolio\RPG_portfolioGameMode.h"
#include "GM_RPG.generated.h"

/**
 * @brief 인게임(월드)에서 사용되는 GameMode
 */
UCLASS()
class RPG_PORTFOLIO_API AGM_RPG : public ARPG_portfolioGameMode
{
	GENERATED_BODY()
	friend class APC_Lobby;

public:
	AGM_RPG();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/**
	* @brief 모든 플레이어의 Dirty데이터를 DB에게 동기화.
	*/
	void SaveAllDirtyPlayerData();


	FTimerHandle SaveTimerHandle;

	float SaveInterval = 300.0f;
};
