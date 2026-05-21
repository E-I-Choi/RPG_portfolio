#pragma once

#include "CoreMinimal.h"
#include "C:\EunIl\Unreal\Projects\C++\MultiPlay\RPG_portfolio\Source\RPG_portfolio\RPG_portfolioPlayerController.h"

#include "GameTypes.h"
#include "MyCharacter.h"

#include "PC_RPG.generated.h"



/** 
* 인게임에서 사용되는 PlayerController
*/
UCLASS()
class RPG_PORTFOLIO_API APC_RPG : public ARPG_portfolioPlayerController
{
	GENERATED_BODY()

public:
	/**
	* @brief 클라이언트 전용 : 서버에서 특정 PC의 캐릭터 세이브 요청을 호출하는 함수입니다.
	* 캐릭터 데이터는 자동으로 차출합니다.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveReqSaveCharacter();

	/**
	* @brief 클라이언트 전용 : 서버에서 특정 PC의 캐릭터 세이브 요청을 호출하는 함수입니다.
	* @param 서버 GI에서 관리 중인 캐릭터 데이터 Depot 내부 데이터.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveReqSaveCharacterWithData(const FCharData& InCharData);

	/**
	* @brief 클라이언트 전용 : 서버에서 특정 PC의 월드 진입을 확인한 뒤 자동으로 호출하는 함수입니다.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveReqRegisterPlayer();

	/**
	* @brief 클라이언트 전용 : 네트워크 통신 직후 자동으로 호출되는 함수입니다.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveNetResponse(const FNetworkReturnResult& Result);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 이름을 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdatName(const FString& NewName);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 경험치를 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateExp(const float ExpIncrease);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 레벨을 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateLevel(const int32 LevelIncrease);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 장비를 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateEquips(const FEquips& NewEquips);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 스킬을 추가합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateSkill(const FString& InSkill, const bool bIsRemove);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 직업을 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateJob(const EClassType InJob);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 스탯을 갱신합니다. 
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateStatus(const FStatus StatIncrease);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 HP를 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateHP(const float HPIncrease);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 MP를 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateMP(const float MPIncrease);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 위치를 갱신합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqUpdateLocation(const int32 X, const int32 Y, const int32 Z);

	/**
	* @brief 서버 전용 : 캐릭터 세이브 통신을 요청합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqSaveCharacter(FCharData InCharData, const FString& InCustomId);

	/**
	* @brief 서버 전용 : 서버 GI Depot에 플레이어 PC 등록을 요청합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_RegisterPlayer(const FString& InstanceId);

	/**
	* @brief 현재 빙의한 캐릭터의 데이터를 반환합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	FCharData GetCharData();
	

};
