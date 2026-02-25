// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkResponseInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "GameTypes.h"

#include "PC_Lobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharListUpdated);


UCLASS()
class RPG_PORTFOLIO_API APC_Lobby : public APlayerController, public INetworkResponseInterface
{
	GENERATED_BODY()
	

public: 
	FOnCharListUpdated OnCharListUpdated;

	virtual void ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result) override;
	virtual void ReceiveCharacterList_Implementation(const TArray<FCharData>& InCharacters) override;

	UFUNCTION(Client, Reliable)
	void Client_NotifyJoinedLobby();

	UFUNCTION(BlueprintNativeEvent)
	void OnJoinedLobbyServer();
};
