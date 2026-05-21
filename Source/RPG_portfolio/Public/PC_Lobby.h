

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "GameTypes.h"

#include "PC_Lobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharListUpdated);

/**
* @brief  로비에서 사용되는 PlayerController
*/
UCLASS()
class RPG_PORTFOLIO_API APC_Lobby : public APlayerController
{
	GENERATED_BODY()
	

public: 

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharListUpdated OnCharListUpdated;

	/** 
	* @brief 서버 전용 : 캐릭터 생성을 요청합니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqCreateNewChar(const FString& InEntityId, const EClassType& InJob, const FString& InName);

	/** 서버 전용 : 캐릭터 리스트 데이터 로드를 요청합니다. */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqLoadAllCharacters(const FString& InEntityId);

	/** 
    * @brief 서버 전용 : 캐릭터 삭제를 요청합니다.
	* 현재 사용하지 않음.
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG/Character")
	void Server_ReqDeleteCharacter();

	/**
	* @brief 클라이언트 전용 : 네트워크 통신 직후 자동으로 호출되는 함수입니다. */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveNetResponse(const FNetworkReturnResult& Result);

	/**
	* @brief 클라이언트 전용 : 캐릭터 데이터 로드시 자동으로 호출되는 함수입니다. */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveCharacterList(const TArray<FCharData>& InCharacters);

};
