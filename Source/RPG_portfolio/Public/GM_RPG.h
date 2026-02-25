// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "PC_Lobby.h"
#include "C:\EunIl\Unreal\Projects\C++\MultiPlay\RPG_portfolio\Source\RPG_portfolio\RPG_portfolioGameMode.h"
#include "GM_RPG.generated.h"

/**
 * 
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

	void SaveAllDirtyPlayerData();

	FTimerHandle SaveTimerHandle;

	float SaveInterval = 300.0f;
};
