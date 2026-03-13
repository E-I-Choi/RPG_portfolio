
#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "NetworkTask.h"
#include "PC_Lobby.h"
#include "PC_RPG.h"
#include "GM_RPG.h"

#include "Core/PlayFabEconomyDataModels.h" 
#include "Core/PlayFabError.h"
#include "GameFramework/GameModeBase.h"

#include "GM_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PORTFOLIO_API AGM_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGM_Lobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void CreateNewCharacter(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext, const EClassType& InJob, const FString& InName);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character")
	void LoadAllCharactersFromServer(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext);
	UFUNCTION()
	UNetworkTask* AddNetworkTask();
	UFUNCTION()
	void RemoveNetworkTask(UNetworkTask* FinishedTask);
	UPROPERTY()
	int32 RequestSequenceIndex;
	UPROPERTY()
	TArray<UNetworkTask*> ActiveTasks;

};
