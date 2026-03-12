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

UENUM(BlueprintType)
enum class ELogSeverity : uint8
{
	Log      UMETA(DisplayName = "Log"),      // 단순 기록용
	Warning  UMETA(DisplayName = "Warning"),  // 주의 (노란색)
	Error    UMETA(DisplayName = "Error")     // 심각한 오류 (빨간색)
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSystemMessageLog, const FString&, Message, ELogSeverity, Severity);


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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSystemMessageLog OnSystemMessageLog;
	UFUNCTION(BlueprintCallable, Category = "System")
	void BroadcastSystemMessage(APlayerController* RequestorPC, const FString& Message, ELogSeverity Severity = ELogSeverity::Log);

	/** Indicates whether CurrentChar contains valid, selected data. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	bool CharSelected = false;
	/** Create new character with job and name. This will automatically send created data to DB server and add it to character list */
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void CreateNewCharacter(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext, const EClassType& InJob, const FString& InName);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void LoadAllCharactersFromServer(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void SaveCharacterToServer(APlayerController* RequestorPC, FCharData InCharData, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	const TArray<FCharData>& GetCharacters() { return Characters; }
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	const FCharData& GetCurrentChar() { return CurrentChar; }

private: 
	UPROPERTY()
	UPlayFabAuthenticationContext* MyAuthContext;
	UPROPERTY()
	TMap<APlayerController*, FCharData> CharDataDepot;
	UPROPERTY()
	FString MyEntityId;
	UPROPERTY()
	FString MyEntityType;
	UPROPERTY()
	int32 RequestSequenceIndex;
	UPROPERTY()
	TArray<UNetworkTask*> ActiveTasks;
	UPROPERTY()
	TArray<FCharData> Characters;
	/** The character data currently selected and controlled by the player. @warning This contains INVALID default values before selection. */
	UPROPERTY()
	FCharData CurrentChar;
	UFUNCTION()
	UNetworkTask* AddNetworkTask();
	UFUNCTION()
	void RemoveNetworkTask(UNetworkTask* FinishedTask);
	UFUNCTION()
	void InitServerPlayFab();
	UFUNCTION(BlueprintCallable, Category = "RPG/Data")
	void SetEntityInfo(const FString& InId, const FString& InType, UPlayFabAuthenticationContext* InAuthContext);

	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void SetCurrentChar(const FCharData& InChar) { CurrentChar = InChar; }
	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	const FString& GetEntityId() { return MyEntityId; }
	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	const FString& GetEntityType() { return MyEntityType; }
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
	UFUNCTION()
	ELogSeverity GetSeverity(FNetworkReturnResult Result);
};

