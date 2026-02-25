// Fill out your copyright notice in the Description page of Project Settings.

#include "PC_RPG.h"
#include "RPGGameInstance.h"

void APC_RPG::ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result)
{
	switch (Result.Type)
	{
	default: break;
	case ENetConnectionType::SaveCharacters:
		return;
	}
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
