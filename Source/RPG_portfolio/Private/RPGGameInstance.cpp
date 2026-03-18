#include "RPGGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"

#include "BPFL_Character.h"
#include "MyCharacter.h"

#include "PlayFab.h"
#include "Core/PlayFabSettings.h"
#include "Core/PlayFabEconomyAPI.h"
#include "Core/PlayFabEconomyDataModels.h"
#include "Core/PlayFabError.h"
#include "NetworkTask.h"

DEFINE_LOG_CATEGORY(LogRPG);

URPGGameInstance::URPGGameInstance()
{
	CustomId = "";
    RequestSequenceIndex = 0;
}

void URPGGameInstance::Init()
{
    Super::Init();

    InitServerPlayFab();

}

void URPGGameInstance::BroadcastSystemMessage(APlayerController* RequestorPC, const FString& Message, ELogSeverity Severity)
{
    switch (Severity)
    {
    case ELogSeverity::Log:
        UE_LOG(LogRPG, Log, TEXT("%s"), *Message);
        break;
    case ELogSeverity::Warning:
        UE_LOG(LogRPG, Warning, TEXT("%s"), *Message);
        if (OnSystemMessageLog.IsBound())
        {
            OnSystemMessageLog.Broadcast(Message, Severity);
        }
        break;
    case ELogSeverity::Error:
        UE_LOG(LogRPG, Error, TEXT("%s"), *Message);
        if (OnSystemMessageLog.IsBound())
        {
            OnSystemMessageLog.Broadcast(Message, Severity);
        }
        break;
    default:
        break;
    }
}

void URPGGameInstance::CreateNewCharacter(APlayerController* RequestorPC, const FString& InEntityId, const EClassType& InJob, const FString& InName)
{
    if (!RequestorPC) return;
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    ANetworkTask* LoadTask = AddNetworkTask();
    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::NewCharacter, TEXT("NewCharacter"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;
    
    LoadTask->ExecuteGrantNewCharItem(InEntityId, InName, InJob);

	return;
}

void URPGGameInstance::LoadAllCharactersFromServer(APlayerController* RequestorPC, const FString& InEntityId)
{
    if (!RequestorPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("InvalidPC"));
        return;
    }
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer)))
    {
        UE_LOG(LogTemp, Warning, TEXT("InvalidNetMode"));
        return;
    }

    ANetworkTask* LoadTask = AddNetworkTask();
    

    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::LoadCharacters, TEXT("CharacterLoad"), 20.0f, RequestSequenceIndex);
    RequestSequenceIndex++;
   

    LoadTask->ExecuteLoadCharacters(InEntityId);
}


void URPGGameInstance::SaveCharacterToServer(APlayerController* RequestorPC, FCharData InCharData, const FString& InCustomId)
{

    if (!RequestorPC) return;
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    ANetworkTask* LoadTask = AddNetworkTask();


    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::SaveCharacters, TEXT("CharacterLoad"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;

    LoadTask->ExecuteUpdateCharData(InCustomId, InCharData);
  
}

void URPGGameInstance::SetPlayFabInfo(const FString& InId)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    CustomId = InId;
    UE_LOG(LogTemp, Log, TEXT("PlayFabCustom ID Saved: %s"), *CustomId);
}

ANetworkTask* URPGGameInstance::AddNetworkTask()
{
    ANetworkTask* NewTask = NewObject<ANetworkTask>(this);
    ActiveTasks.Add(NewTask);
    NewTask->OnPostTaskDestroy.BindUObject(this, &URPGGameInstance::RemoveNetworkTask);

    return NewTask;
}

void URPGGameInstance::RemoveNetworkTask(ANetworkTask* FinishedTask)
{
    if (FinishedTask)
    {
        ActiveTasks.Remove(FinishedTask);
        UE_LOG(LogTemp, Log, TEXT("Task removed from GI array. GC will collect it soon."));
    }
}

void URPGGameInstance::InitServerPlayFab()
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer))
    {
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
}


void URPGGameInstance::UpdateCharNameToDepot(APlayerController* PC, const FString& InName)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;
    
    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Name = InName;
    TargetData->SetDirty(EDF_Name);

    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->ConditionComponent -> CharName = InName;
}

void URPGGameInstance::UpdateCharExpToDepot(APlayerController* PC, const float& ExpIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    if (TargetData->Exp + ExpIncrease >= 100.0f)
    {

        UpdateCharLevelToDepot(PC, 1);
        TargetData->Exp = TargetData->Exp + ExpIncrease - 100.0f;
    }
    else if (TargetData->Exp + ExpIncrease < 0.0f)
    {
        TargetData->Exp = 0.0f;
    }
    else
    {
        TargetData->Exp = TargetData->Exp + ExpIncrease;
    }

    TargetData->SetDirty(EDF_Exp);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->Exp = TargetData->Exp;

}

void URPGGameInstance::UpdateCharLevelToDepot(APlayerController* PC, const int32& LevelIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Level = TargetData->Level + LevelIncrease;
    TargetData->SetDirty(EDF_Level);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    if(!(TheChar->ConditionComponent))
    {
        return;
    }
    TheChar->ConditionComponent->Level = TargetData->Level;
}

void URPGGameInstance::UpdateCharEquipToDepot(APlayerController* PC, const FEquips& InEquips)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Equips = InEquips;
    TargetData->SetDirty(EDF_Equips);
    
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    //현재는 Equips를 묶어서 한 번에 처리하지만 확장성을 고려하여 로직을 각 파츠 단위로 쪼갤 수 있음
    TheChar->Hat = TargetData->Equips.Hat;
    TheChar->Top = TargetData->Equips.Top;
    TheChar->Pants = TargetData->Equips.Pants;
    TheChar->Weapon = TargetData->Equips.Weapon;
    TheChar->Shoes = TargetData->Equips.Shoes;
}

void URPGGameInstance::UpdateCharSkillToDepot(APlayerController* PC, const FString& InSkill, bool bIsRemove)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    bool bExists = TargetData->Skills.ContainsByPredicate([&](const FString& Element)
        {
            return Element == InSkill;
        });
    if (bExists && bIsRemove)
    {
        TargetData->Skills.RemoveSwap(InSkill, true);
    }
    else if (!bExists && !bIsRemove)
    {
        TargetData->Skills.Add(InSkill);
    }

    TargetData->SetDirty(EDF_Skills);

    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->Skills = TargetData->Skills;
}

void URPGGameInstance::UpdateCharJobToDepot(APlayerController* PC, const EClassType& NewClass)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Job = NewClass;
    TargetData->SetDirty(EDF_Job);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->Job = TargetData->Job;
}

void URPGGameInstance::UpdateCharStatusToDepot(APlayerController* PC, const FStatus& StatIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* CharData = CharDataDepot.Find(PC);

    if (CharData == nullptr) return;

    FStatus* TargetData = &(CharData->Status);

    TargetData->Agility = TargetData->Agility + StatIncrease.Agility;
    TargetData->Health = TargetData->Health + StatIncrease.Health;
    TargetData->Intelligence = TargetData->Intelligence + StatIncrease.Intelligence;
    TargetData->Strength = TargetData->Strength + StatIncrease.Strength;

    CharData->SetDirty(EDF_Status);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->Status = CharData->Status;
}

void URPGGameInstance::UpdateCharHPToDepot(APlayerController* PC, const float& HPIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->HP = TargetData->HP + HPIncrease;
    TargetData->SetDirty(ECharacterDirtyFlags::EDF_HP);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    if (!(TheChar->ConditionComponent))
    {
        return;
    }
    TheChar->ConditionComponent->HP = TargetData->HP;
    
}

void URPGGameInstance::UpdateCharMPToDepot(APlayerController* PC, const float& MPIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->HP = TargetData->MP + MPIncrease;
    TargetData->SetDirty(ECharacterDirtyFlags::EDF_MP);
   
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    if (!(TheChar->ConditionComponent))
    {
        return;
    }
    TheChar->ConditionComponent->MP = TargetData->MP;
    
}

void URPGGameInstance::UpdateCharLocationToDepot(APlayerController* PC, const int32& X, const int32& Y, const int32& Z)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->LocX = X;
    TargetData->LocY = Y;
    TargetData->LocZ = Z;
}

void URPGGameInstance::RegisterPlayer(APlayerController* PC, const FString& InId)
{
    FCharData NewData;
    NewData.SetItemInstanceId(InId);
    CharDataDepot.Add(PC, NewData);
}

ELogSeverity URPGGameInstance::GetSeverity(FNetworkReturnResult Result)
{
    ELogSeverity Severity = ELogSeverity::Log;
    switch (Result.Type)
    {
    default:break;
    case ENetConnectionType::NewCharacter:
        if (Result.Response == ENetResponseType::Success)
        {
            Severity = ELogSeverity::Log;
        }
        else
        {
            Severity = ELogSeverity::Warning;
        }
    case ENetConnectionType::LoadCharacters:
        if (Result.Response == ENetResponseType::Success)
        {
            Severity = ELogSeverity::Log;
        }
        else
        {
            Severity = ELogSeverity::Error;
        }
    case ENetConnectionType::SaveCharacters:
        if (Result.Response == ENetResponseType::Success)
        {
            Severity = ELogSeverity::Log;
        }
        else
        {
            Severity = ELogSeverity::Error;
        }
    }
    return Severity;
}
