// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkResponseInterface.h"
#include "CoreMinimal.h"
#include "C:\EunIl\Unreal\Projects\C++\MultiPlay\RPG_portfolio\Source\RPG_portfolio\RPG_portfolioPlayerController.h"

#include "GameTypes.h"
#include "MyCharacter.h"

#include "PC_RPG.generated.h"




UCLASS()
class RPG_PORTFOLIO_API APC_RPG : public ARPG_portfolioPlayerController, public INetworkResponseInterface
{
	GENERATED_BODY()

public:
	virtual void ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result) override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdatName(const FString& NewName);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateExp(const float ExpIncrease);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateLevel(const int32 LevelIncrease);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateEquips(const FEquips& NewEquips);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateSkill(const FString& InSkill, const bool bIsRemove);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateJob(const EClassType InJob);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateStatus(const FStatus StatIncrease);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateHP(const float HPIncrease);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateMP(const float MPIncrease);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	FCharData GetCharData();
	

};
