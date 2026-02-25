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
	MyEntityId = "";
    MyEntityType = "";
    RequestSequenceIndex = 0;
}

void URPGGameInstance::Init()
{
    Super::Init();

    InitServerPlayFab();
}

void URPGGameInstance::BroadcastSystemMessage(const FString& Message, ELogSeverity Severity)
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

void URPGGameInstance::CreateNewCharacter(APlayerController* RequestorPC, const EClassType& InJob, const FString& InName)
{
    if (!RequestorPC) return;
    if ((GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    UNetworkTask* LoadTask = AddNetworkTask();
    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::NewCharacter, TEXT("NewCharacter"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;
    URPGGameInstance* RequestorGI = Cast<URPGGameInstance>(RequestorPC->GetGameInstance());
    FString EntityId = TEXT("");
    FString EntityType = TEXT("");
    if (RequestorGI)
    {
        EntityId = RequestorGI->MyEntityId;
        EntityType = RequestorGI->MyEntityType;
    }

    LoadTask->ExecuteGrantNewCharItem(EntityId, EntityType, InName, InJob);

	return;
}

void URPGGameInstance::LoadAllCharactersFromServer(APlayerController* RequestorPC)
{
    if (!RequestorPC) return;
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    UNetworkTask* LoadTask = AddNetworkTask();
    

    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::LoadCharacters, TEXT("CharacterLoad"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;
    URPGGameInstance* RequestorGI = Cast<URPGGameInstance>(RequestorPC->GetGameInstance());
    FString EntityId = TEXT("");
    FString EntityType = TEXT("");
    if (RequestorGI)
    {
        EntityId = RequestorGI->MyEntityId;
        EntityType = RequestorGI->MyEntityType;
    }

    LoadTask->ExecuteLoadCharacters(EntityId, EntityType);
}


void URPGGameInstance::SaveCharacterToServer(APlayerController* RequestorPC, FCharData InCharData)
{

    if (!RequestorPC) return;
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    UNetworkTask* LoadTask = AddNetworkTask();


    LoadTask->InitUNetworkTask(RequestorPC, ENetConnectionType::SaveCharacters, TEXT("CharacterLoad"), 10.0f, RequestSequenceIndex);
    RequestSequenceIndex++;
    URPGGameInstance* RequestorGI = Cast<URPGGameInstance>(RequestorPC->GetGameInstance());
    FString EntityId = TEXT("");
    FString EntityType = TEXT("");
    if (RequestorGI)
    {
        EntityId = RequestorGI->MyEntityId;
        EntityType = RequestorGI->MyEntityType;
    }

    LoadTask->ExecuteUpdateCharData(EntityId, EntityType, InCharData);
  
}

void URPGGameInstance::UpdateCharName(const FString& InName)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];
    
    TargetData.Name = InName;
}

void URPGGameInstance::UpdateCharExp(const float& NewExp)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];

    TargetData.Exp = NewExp;
   
}

void URPGGameInstance::UpdateCharLevel(const int32& NewLevel)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];

    TargetData.Level = NewLevel;
}

void URPGGameInstance::UpdateCharEquip(const FEquips& InEquips)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];

    TargetData.Equips = InEquips;
}

void URPGGameInstance::UpdateCharSkill(const TArray<FString>& InSkills)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];

    TargetData.Skills = InSkills;
}

void URPGGameInstance::UpdateCharJob(const EClassType& NewClass)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FCharData& TargetData = Characters[CurrentCharIndex];

    TargetData.Job = NewClass;
}

void URPGGameInstance::UpdateCharStatus(const FStatus& NewStat)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    FStatus& TargetData = Characters[CurrentCharIndex].Status;

    TargetData = NewStat;
}

void URPGGameInstance::SetEntityInfo(const FString& InId, const FString& InType)
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;
    MyEntityId = InId;
    MyEntityType = InType;
    UE_LOG(LogTemp, Log, TEXT("Entity ID Saved: %s"), *MyEntityId);
}

UNetworkTask* URPGGameInstance::AddNetworkTask()
{
    UNetworkTask* NewTask = NewObject<UNetworkTask>(this);
    ActiveTasks.Add(NewTask);
    NewTask->OnPostTaskDestroy.BindUObject(this, &URPGGameInstance::RemoveNetworkTask);

    return NewTask;
}

void URPGGameInstance::RemoveNetworkTask(UNetworkTask* FinishedTask)
{
    if (FinishedTask)
    {
        if (FinishedTask->ConnectionType == ENetConnectionType::NewCharacter)
        { //캐릭터 생성 이후 다시 캐릭터 목록 업데이트. 대형 프로젝트가 될 경우 효율을 고려하여
          //방금 생성한 캐릭터만 업데이트하는 Fetch 함수를 따로 만들 수 있음
            LoadAllCharactersFromServer(Cast<APlayerController>(FinishedTask->RequestorPC));
        }
        ActiveTasks.Remove(FinishedTask);
        UE_LOG(LogTemp, Log, TEXT("Task removed from GI array. GC will collect it soon."));
    }
}

void URPGGameInstance::InitServerPlayFab()
{
    if (GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GetMutableDefault<UPlayFabRuntimeSettings>()->DeveloperSecretKey = TEXT("CMOPICH4F873IPQCOKS8ZF9T7TR84HFGEJYCY6IXH5T3GFTYMI");
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
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharName(InName);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    TheChar->CharName = InName;
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
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharExp(TargetData->Exp);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    //TheChar EXP도 업데이트 해줘야됨

}

void URPGGameInstance::UpdateCharLevelToDepot(APlayerController* PC, const int32& LevelIncrease)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Level = TargetData->Level + LevelIncrease;
    TargetData->SetDirty(EDF_Level);
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharLevel(TargetData->Level);
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
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI -> UpdateCharEquip(InEquips);
    
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

    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharSkill(TargetData->Skills);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    //TheChar -
}

void URPGGameInstance::UpdateCharJobToDepot(APlayerController* PC, const EClassType& NewClass)
{
    if (!(GetWorld()->IsNetMode(NM_DedicatedServer))) return;

    if (!PC) return;
    FCharData* TargetData = CharDataDepot.Find(PC);

    if (TargetData == nullptr) return;

    TargetData->Job = NewClass;
    TargetData->SetDirty(EDF_Job);
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharJob(NewClass);
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
    URPGGameInstance* GI = Cast<URPGGameInstance>(PC->GetGameInstance());
    GI->UpdateCharStatus(CharData->Status);
    AMyCharacter* TheChar = Cast<AMyCharacter>(PC->GetPawn());
    //TheChar -
}

void URPGGameInstance::RegisterPlayer(APlayerController* PC, const FString& InId)
{
    FCharData NewData;
    NewData.SetItemInstanceId(InId);
    CharDataDepot.Add(PC, NewData);
}

void URPGGameInstance::CleanUpCharData(APlayerController* PC)
{
    CharDataDepot.Find(PC)->ClearDirty();
}
