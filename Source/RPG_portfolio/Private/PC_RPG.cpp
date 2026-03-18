// Fill out your copyright notice in the Description page of Project Settings.

#include "PC_RPG.h"
#include "RPGGameInstance.h"

void APC_RPG::Client_ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->OnSystemMessageLog.Broadcast(Result.Context, GI->GetSeverity(Result));
}

bool APC_RPG::Server_ReqUpdateSkill_Validate(const FString& InSkill, const bool bIsRemove)
{
	return true;
}

void APC_RPG::Server_ReqUpdateSkill_Implementation(const FString& InSkill, const bool bIsRemove)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharSkillToDepot(this, InSkill, bIsRemove);
}

bool APC_RPG::Server_ReqUpdateEquips_Validate(const FEquips& NewEquips)
{
	return true;
}

void APC_RPG::Server_ReqUpdateEquips_Implementation(const FEquips& NewEquips)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharEquipToDepot(this, NewEquips);
}

bool APC_RPG::Server_ReqUpdateLevel_Validate(const int32 LevelIncrease)
{
	return true;
}

void APC_RPG::Server_ReqUpdateLevel_Implementation(const int32 LevelIncrease)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharLevelToDepot(this, LevelIncrease);
}

bool APC_RPG::Server_ReqUpdateExp_Validate(const float ExpIncrease)
{
	return true;
}

void APC_RPG::Server_ReqUpdateExp_Implementation(const float ExpIncrease)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharExpToDepot(this, ExpIncrease);
}

void APC_RPG::Server_ReqUpdatName_Implementation(const FString& NewName)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharNameToDepot(this, NewName);
}

bool APC_RPG::Server_ReqUpdatName_Validate(const FString& NewName)
{
	return true;
}

bool APC_RPG::Server_ReqUpdateJob_Validate(const EClassType InJob)
{
	return true;
}

void APC_RPG::Server_ReqUpdateJob_Implementation(const EClassType InJob)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharJobToDepot(this, InJob);
}

bool APC_RPG::Server_ReqUpdateStatus_Validate(const FStatus StatIncrease)
{
	return true;
}

void APC_RPG::Server_ReqUpdateStatus_Implementation(const FStatus StatIncrease)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharStatusToDepot(this, StatIncrease);
}

void APC_RPG::Server_ReqUpdateHP_Implementation(const float HPIncrease)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharHPToDepot(this, HPIncrease);
}

void APC_RPG::Server_ReqUpdateMP_Implementation(const float MPIncrease)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->UpdateCharMPToDepot(this, MPIncrease);
}

bool APC_RPG::Server_ReqUpdateHP_Validate(const float HPIncrease)
{
	return true;
}

bool APC_RPG::Server_ReqUpdateMP_Validate(const float MPIncrease)
{
	return true;
}

bool APC_RPG::Server_ReqUpdateLocation_Validate(const int32 X, const int32 Y, const int32 Z)
{
	return true;
}

void APC_RPG::Server_ReqUpdateLocation_Implementation(const int32 X, const int32 Y, const int32 Z)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	AMyCharacter* MyPawn = Cast<AMyCharacter>(GetPawn());
	GI->UpdateCharLocationToDepot(this, FMath::CeilToInt32(MyPawn->GetActorLocation().X), FMath::CeilToInt32(MyPawn->GetActorLocation().Y), FMath::CeilToInt32(MyPawn->GetActorLocation().Z));
}

void APC_RPG::Server_ReqSaveCharacter_Implementation(FCharData InCharData, const FString& InCustomId)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->SaveCharacterToServer(this, InCharData, InCustomId);

}

bool APC_RPG::Server_ReqSaveCharacter_Validate(FCharData InCharData, const FString& InCustomId)
{
	return true;
}

void APC_RPG::Server_RegisterPlayer_Implementation(const FString& InstanceId)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->RegisterPlayer(this, InstanceId);
}

bool APC_RPG::Server_RegisterPlayer_Validate(const FString& InstanceId)
{
	return true;
}

FCharData APC_RPG::GetCharData()
{
	FCharData CharData;
	AMyCharacter* MyPawn = Cast<AMyCharacter>(GetPawn());
	if (!MyPawn) return CharData;
	FCharData GICharData = Cast<URPGGameInstance>(GetGameInstance())->CurrentChar;
	FEquips CurrentEquips;
	CurrentEquips.Hat = MyPawn->Hat;
	CurrentEquips.Pants = MyPawn->Pants;
	CurrentEquips.Shoes = MyPawn->Shoes;
	CurrentEquips.Top = MyPawn->Top;
	CurrentEquips.Weapon = MyPawn->Weapon;
	CharData.Equips= CurrentEquips;
	CharData.Exp = MyPawn->Exp;
	CharData.HP = MyPawn->ConditionComponent->HP;
	CharData.Job = MyPawn->Job;
	CharData.Level = MyPawn->ConditionComponent->Level;
	CharData.MP = MyPawn->ConditionComponent->MP;
	CharData.Name = MyPawn->ConditionComponent->CharName;
	CharData.SetItemInstanceId(GICharData.GetItemInstanceId());
	CharData.Skills = MyPawn->Skills;
	CharData.Status = MyPawn->Status;
	CharData.LocX = FMath::CeilToInt32(MyPawn->GetActorLocation().X);
	CharData.LocY = FMath::CeilToInt32(MyPawn->GetActorLocation().Y);
	CharData.LocZ = FMath::CeilToInt32(MyPawn->GetActorLocation().Z);
	return CharData;
}

void APC_RPG::Client_ReceiveReqSaveCharacter_Implementation()
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	Server_ReqSaveCharacter(GetCharData(), GI->GetCustomId());
}

void APC_RPG::Client_ReceiveReqSaveCharacterWithData_Implementation(const FCharData& InCharData)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	Server_ReqSaveCharacter(InCharData, GI->GetCustomId());
}

void APC_RPG::Client_ReceiveReqRegisterPlayer_Implementation()
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	FCharData TheCharData = GI->GetCurrentChar();
	Server_RegisterPlayer(TheCharData.GetItemInstanceId());
}
