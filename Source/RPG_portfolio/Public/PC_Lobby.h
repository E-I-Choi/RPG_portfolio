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

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqCreateNewChar();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqLoadAllCharacters();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqDeleteCharacter(); // 여기엔 식별 인자 필요함

	virtual void ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result) override;
	UFUNCTION(Client, Reliable)
	void ReceiveCharacterList(const TArray<FCharData>& InCharacters);
	UFUNCTION(Client, Reliable)
	void PostLoginLoadCharacters();

	UFUNCTION(BlueprintNativeEvent)
	void OnJoinedLobbyServer();
};
