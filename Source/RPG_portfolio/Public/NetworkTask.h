#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameTypes.h"
#include "PlayFab.h"


#include "PlayFabEconomyDataModels.h"
#include "PlayFabSettings.h"

#include "NetworkTask.generated.h"

enum class ETaskState : uint8
{
	Preparing,
	InFlight,
	TimedOut,
	Finished
};

DECLARE_DELEGATE_OneParam(FOnPostTaskDestroy, ANetworkTask*);

UCLASS()
class RPG_PORTFOLIO_API ANetworkTask : public AActor
{
	GENERATED_BODY()
	
public:

	FOnPostTaskDestroy OnPostTaskDestroy;

	UFUNCTION()
	void InitUNetworkTask(TWeakObjectPtr<class APlayerController> Requestor, ENetConnectionType Type, FString InContext, float InTimeLimit, int32 InTicketId);
	UFUNCTION()
	void ExecuteTimer();
	UFUNCTION()
	void ExecuteLoadCharacters(FString InEntityId);
	UFUNCTION()
	void ExecuteUpdateCharData(FString InCustomId, FCharData DataToSave);
	UFUNCTION()
	void ExecuteGrantNewCharItem(FString InCustomId, FString InName, EClassType InJob);
	ENetConnectionType ConnectionType = ENetConnectionType::none;
	TWeakObjectPtr<class APlayerController> RequestorPC;
	

private :

	float TimeLimit = 0.0f;
	ETaskState CurrentState = ETaskState::Preparing;
	FString Context = TEXT("");
	int32 TicketId = 0;
	FTimerHandle TimeoutHandle;
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
