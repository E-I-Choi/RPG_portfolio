#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameTypes.h"
#include "PlayFab.h"


#include "PlayFabEconomyDataModels.h"
#include "PlayFabSettings.h"

#include "NetworkTask.generated.h"

/**
* @brief NetworkTask의 현재 상태 유형
*/
enum class ETaskState : uint8
{
	Preparing,
	InFlight,
	TimedOut,
	Finished
};

DECLARE_DELEGATE_OneParam(FOnPostTaskDestroy, ANetworkTask*);


/**
* @brief DB(PlayFab)과 통신하는 모든 작업을 수행하는 AActor 오브젝트
* LogIn, Load, Save, Grant, Delete(사용하지 않음) 기능 포함
* 현재 LogIn, Load까지는 정상 작동하나 Save와 Grant기능이 정상 작동하지 않음
* SDK 버전 업데이트 후 이에 맞춰 모든 함수를 수정할 예정
*/
UCLASS()
class RPG_PORTFOLIO_API ANetworkTask : public AActor
{
	GENERATED_BODY()
	
public:

	FOnPostTaskDestroy OnPostTaskDestroy;

	/**
	* @brief NetWork Task가 동작하기 위해 필요한 정보들을 미리 초기화하는 함수
	* @param InContext 타임아웃 발생 시 출력할 로그를 위한 내용
	* @param InTicketId 타임아웃 발생 시 출력할 로그를 위한 통신 오더 식별자
	*/
	UFUNCTION()
	void InitUNetworkTask(TWeakObjectPtr<class APlayerController> Requestor, ENetConnectionType Type, FString InContext, float InTimeLimit, int32 InTicketId);
	
	/**
	* @brief TimeOut을 판별하기 위해 통신 직전에 호출하는 함수 (非필수 요소)
	* 현재 로비에서 통신 성패 여부와는 별개로 There is no world 오류가 발생 - 해결 예정
	*/
	UFUNCTION()
	void ExecuteTimer();

	/**
	* @brief 요청자의 모든 캐릭터 데이터를 로드합니다.
	*/
	UFUNCTION()
	void ExecuteLoadCharacters(FString InEntityId);

	/**
	* @brief 요청자의 현재 캐릭터 데이터를 DB에 동기화합니다.
	*/
	UFUNCTION()
	void ExecuteUpdateCharData(FString InCustomId, FCharData DataToSave);

	/**
	* @brief 요청자의 DB에 새로운 캐릭터를 생성합니다.
	*/
	UFUNCTION()
	void ExecuteGrantNewCharItem(FString InCustomId);
	ENetConnectionType ConnectionType = ENetConnectionType::none;
	TWeakObjectPtr<class APlayerController> RequestorPC;
	

private :

	float TimeLimit = 0.0f;
	ETaskState CurrentState = ETaskState::Preparing;
	FString Context = TEXT("");
	int32 TicketId = 0;
	FTimerHandle TimeoutHandle;

	/**
	* @brief 작업을 종료하고 로그를 출력한 뒤, 액터를 소멸시킵니다.
	*/
	void FinishTask(bool bSuccess);
	
	
	UFUNCTION()
	void OnSucessLoadCharacters(FServerGetUserInventoryResult Result, UObject* CustomData);
	UFUNCTION()
	void OnSucessGrantCharacter(FServerGrantItemsToUserResult Result, UObject* CustomData);
	UFUNCTION()
	void OnSucessUpdateCharacter(FServerEmptyResponse Result, UObject* CustomData);
	UFUNCTION()
	void OnLoadFailure(FPlayFabError Error, UObject* CustomData);
	UFUNCTION()
	void OnGrantFailure(FPlayFabError Error, UObject* CustomData);
	UFUNCTION()
	void OnUpdateFailure(FPlayFabError Error, UObject* CustomData);

};
