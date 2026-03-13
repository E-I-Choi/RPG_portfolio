#include "GM_Lobby.h"
#include "RPGGameInstance.h"
#include "PC_Lobby.h"

AGM_Lobby::AGM_Lobby()
{
}

void AGM_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);


}

void AGM_Lobby::BeginPlay()
{
    Super::BeginPlay();

    FString TargetTitleId;
    FString TargetSecretKey;

    GConfig->GetString(TEXT("/Script/PlayFabSettings"), TEXT("TitleId"), TargetTitleId, GGameIni);
    GConfig->GetString(TEXT("/Script/PlayFabSettings"), TEXT("DeveloperSecretKey"), TargetSecretKey, GGameIni);

    if (!TargetTitleId.IsEmpty() && !TargetSecretKey.IsEmpty())
    {
        UPlayFabRuntimeSettings* Settings = GetMutableDefault<UPlayFabRuntimeSettings>();

        if (Settings)
        {
            Settings->TitleId = TargetTitleId;
            Settings->DeveloperSecretKey = TargetSecretKey;

            UE_LOG(LogTemp, Log, TEXT("PlayFab Server Settings Initialized."));
        }
    }
}

void AGM_Lobby::CreateNewCharacter(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext, const EClassType& InJob, const FString& InName)
{
    if (!RequestorPC) return;
    if ((GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    UNetworkTask* LoadTask = AddNetworkTask();
    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::NewCharacter, TEXT("NewCharacter"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;

    LoadTask->ExecuteGrantNewCharItem(InEntityId, InEntityType, InAuthContext, InName, InJob);

    return;
}

void AGM_Lobby::LoadAllCharactersFromServer(APlayerController* RequestorPC, const FString& InEntityId, const FString& InEntityType, UPlayFabAuthenticationContext* InAuthContext)
{
    if (!RequestorPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("InvalidPC"));
        return;
    }
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer)))
    {
        UE_LOG(LogTemp, Warning, TEXT("InvalidOwner"));
        return;
    }

    UNetworkTask* LoadTask = AddNetworkTask();


    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::LoadCharacters, TEXT("CharacterLoad"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;


    LoadTask->ExecuteLoadCharacters(InEntityId, InEntityType, InAuthContext);
}

UNetworkTask* AGM_Lobby::AddNetworkTask()
{
    UNetworkTask* NewTask = NewObject<UNetworkTask>(this);
    ActiveTasks.Add(NewTask);
    NewTask->OnPostTaskDestroy.BindUObject(this, &AGM_Lobby::RemoveNetworkTask);

    return NewTask;
}

void AGM_Lobby::RemoveNetworkTask(UNetworkTask* FinishedTask)
{
    if (FinishedTask)
    {
        ActiveTasks.Remove(FinishedTask);
        UE_LOG(LogTemp, Log, TEXT("Task removed from GI array. GC will collect it soon."));
    }
}