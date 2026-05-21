#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameTypes.h"
#include "NetworkTask.h"
#include "PC_Lobby.h"
#include "PC_RPG.h"
#include "GM_Lobby.h"
#include "GM_RPG.h"

#include "Core/PlayFabEconomyDataModels.h" 
#include "Core/PlayFabError.h"

#include "RPGGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPG, Log, All);

/** 
* @brief 로그 유형
*/
UENUM(BlueprintType)
enum class ELogSeverity : uint8
{
	Log      UMETA(DisplayName = "Log"),      // 단순 기록용
	Warning  UMETA(DisplayName = "Warning"),  // 주의 (노란색)
	Error    UMETA(DisplayName = "Error")     // 심각한 오류 (빨간색)
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSystemMessageLog, const FString&, Message, ELogSeverity, Severity);

/**
* @brief 게임의 초기화와 데이터 Depot 역할을 담당하는 기초 뼈대
* Server(Dedicated), Local에 따라 역할이 나뉘며 각 함수에서 조건문으로 분기 혹은 호출 이전에 조건으로 분기
* 여담 : 작업이 완료된 시점에서 Server 역할은 GM으로 빼는 게 직관적이고 깔끔하며 분류상 적합하다고 느끼지만
* 시간 관계상 수정하지 않음
*/
UCLASS()
class RPG_PORTFOLIO_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	friend class APC_RPG;
	friend class APC_Lobby;
	friend class AGM_RPG;
	friend class AGM_Lobby;
	
public:
	URPGGameInstance();

	virtual void Init() override;

	/** 커스텀 시스템 메시지 로그 발생 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSystemMessageLog OnSystemMessageLog;

	/** 커스텀 시스템 메시지 중 위젯을 통해 표시가 필요한 내용을 전달함. */
	UFUNCTION(BlueprintCallable, Category = "System")
	void BroadcastSystemMessage(APlayerController* RequestorPC, const FString& Message, ELogSeverity Severity = ELogSeverity::Log);

	/** 현재 플레이어가 유효한 캐릭터를 선택했는지 판별하는 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	bool CharSelected = false;

	/**
	* @brief 직업과 이름을 입력하여 새 캐릭터를 생성합니다.
	* 생성된 데이터는 자동으로 데이터베이스 서버로 전송되어 캐릭터 목록에 추가됩니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void CreateNewCharacter(APlayerController* RequestorPC, const FString& InEntityId, const EClassType& InJob, const FString& InName);
	
	/**
	* @brief DB로부터 RequestorPC의 모든 플레이어 캐릭터 데이터를 로드합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void LoadAllCharactersFromServer(APlayerController* RequestorPC, const FString& InEntityId);

	/**
	* @brief 현재 캐릭터 데이터를 DB에 저장합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void SaveCharacterToServer(APlayerController* RequestorPC, FCharData InCharData, const FString& InEntityId);
	
	/**
	* @brief 로컬 GI에 저장된 캐릭터 리스트 데이터를 반환합니다. 이 데이터는 로드 시점의 초기값입니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	const TArray<FCharData>& GetCharacters() { return Characters; }
	
	/**
	* @brief 로컬 GI에 저장된 캐릭터 데이터를 반환합니다. 이 데이터는 로드 시점의 초기값입니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	const FCharData& GetCurrentChar() { return CurrentChar; }

private: 

	/**
	* @brief 서버에만 존재하는 PC : 캐릭터 데이터 TMap 데이터 저장소입니다.
	* GM Timer에 의해 300초 간격으로 저장됩니다.
	* 각 PC에서 업데이트 요청 시 이 변수에 모든 데이터가 갱신됩니다.
	*/
	UPROPERTY()
	TMap<APlayerController*, FCharData> CharDataDepot;

	/**
	* @brief 로컬 GI에 저장된 플레이어의 커스텀 아이디입니다.
	*/
	UPROPERTY()
	FString CustomId;

	/**
	* @brief 네트워크 통신 시도 회차
	*/
	UPROPERTY()
	int32 RequestSequenceIndex;

	/**
	* @brief 서버에서 현재 운용 중인 네트워크 테스크 액터 포인터 배열입니다.
	*/
	UPROPERTY()
	TArray<ANetworkTask*> ActiveTasks;

	/**
	* @brief 로컬 GI에 저장된 캐릭터 리스트 데이터입니다. 로드 직후의 초기값입니다.
	*/
	UPROPERTY()
	TArray<FCharData> Characters;

	/**
	* @brief 로컬 GI에 저장된 캐릭터 데이터입니다. 로드 직후의 초기값입니다.
	*/
	UPROPERTY()
	FCharData CurrentChar;

	/**
	* @brief 네트워크 통신을 위해 네트워크 테스크를 생성하고 배열에 추가하고, 발동시킵니다.
	*/
	UFUNCTION()
	ANetworkTask* AddNetworkTask();

	/**
	* @brief 네트워크 테스트를 배열에서 제거합니다.
	*/
	UFUNCTION()
	void RemoveNetworkTask(ANetworkTask* FinishedTask);

	/**
	* @brief 플레이팹 사용을 위해 필요한 모든 값을 초기화합니다.
	*/
	UFUNCTION()
	void InitServerPlayFab();

	/**
	* @brief 로컬 GI에 PlayFab CustomId를 저장합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Data")
	void SetPlayFabInfo(const FString& InId);

	/**
	* @brief 로컬 GI에 현재 캐릭터 데이터를 특정하여 저장합니다.
	*캐릭터 선택 이후 월드 진입 시점에 호출합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void SetCurrentChar(const FCharData& InChar) { CurrentChar = InChar; }

	/**
	* @brief 로컬 GI에 저장된 CustomId를 반환합니다.
	*/
	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	const FString& GetCustomId() { return CustomId; }

	UFUNCTION()
	void UpdateCharNameToDepot(APlayerController* PC, const FString& InName);
	UFUNCTION()
	void UpdateCharExpToDepot(APlayerController* PC, const float& ExpIncrease);
	UFUNCTION()
	void UpdateCharLevelToDepot(APlayerController* PC, const int32& LevelIncrease);
	UFUNCTION()
	void UpdateCharEquipToDepot(APlayerController* PC, const FEquips& InEquips);
	UFUNCTION()
	void UpdateCharSkillToDepot(APlayerController* PC, const FString& InSkill, bool bIsRemove = false);
	UFUNCTION()
	void UpdateCharJobToDepot(APlayerController* PC, const EClassType& NewClass);
	UFUNCTION()
	void UpdateCharStatusToDepot(APlayerController* PC, const FStatus& StatIncrease);
	UFUNCTION()
	void UpdateCharHPToDepot(APlayerController* PC, const float& HPIncrease);
	UFUNCTION()
	void UpdateCharMPToDepot(APlayerController* PC, const float& MPIncrease);
	UFUNCTION()
	void UpdateCharLocationToDepot(APlayerController* PC, const int32& X, const int32& Y, const int32& Z);
	UFUNCTION()
	void RegisterPlayer(APlayerController* PC, const FString& InId);

	/**
	* @brief 네트워크 통신 결과에 따라 심각성을 반환
	*/
	UFUNCTION()
	ELogSeverity GetSeverity(FNetworkReturnResult Result);
};

