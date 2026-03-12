// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "GameTypes.h"

#include "PC_Lobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharListUpdated);


UCLASS()
class RPG_PORTFOLIO_API APC_Lobby : public APlayerController
{
	GENERATED_BODY()
	

public: 

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharListUpdated OnCharListUpdated;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqCreateNewChar(const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext, const EClassType& InJob, const FString& InName);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqLoadAllCharacters(const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqDeleteCharacter(); // 사용하지 않음

	UFUNCTION(Client, Reliable)
	void Client_ReceiveNetResponse(const FNetworkReturnResult& Result);
	UFUNCTION(Client, Reliable)
	void Client_ReceiveCharacterList(const TArray<FCharData>& InCharacters);

};
