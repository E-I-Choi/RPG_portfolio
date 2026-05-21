#include "PC_Lobby.h"
#include "RPGGameInstance.h"



/* 플레이어의 캐릭터 생성 요청이 적절한지 검토합니다. 현재 Always true */
bool APC_Lobby::Server_ReqCreateNewChar_Validate(const FString& InEntityId, const EClassType& InJob, const FString& InName)
{
	return true;
}

void APC_Lobby::Server_ReqCreateNewChar_Implementation(const FString& InEntityId, const EClassType& InJob, const FString& InName)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->CreateNewCharacter(this, InEntityId, InJob, InName);
}

/*  플레이어의 데이터 로드 요청이 적절한지 검토합니다. 현재 Always true */
bool APC_Lobby::Server_ReqLoadAllCharacters_Validate(const FString& InEntityId)
{
	return true;
}

void APC_Lobby::Server_ReqLoadAllCharacters_Implementation(const FString& InEntityId)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->LoadAllCharactersFromServer(this, InEntityId);
}

/*  플레이어의 캐릭터 삭제 요청이 적절한지 검토합니다. 현재 Always true */
bool APC_Lobby::Server_ReqDeleteCharacter_Validate()
{
	return true;
}

void APC_Lobby::Server_ReqDeleteCharacter_Implementation()
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
}



void APC_Lobby::Client_ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result)
{
	/* 캐릭터 생성 직후, 방금 생성한 캐릭터 데이터를 포함해 다시 모든 캐릭터 정보를 로드합니다.*/
	switch (Result.Type)
	{
	default: break;
	case ENetConnectionType::NewCharacter:
		if (Result.Response == ENetResponseType::Success)
		{
			URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
			Server_ReqLoadAllCharacters(GI->GetCustomId());
		}
		break;
	}

	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->OnSystemMessageLog.Broadcast(Result.Context, GI->GetSeverity(Result));
}


void APC_Lobby::Client_ReceiveCharacterList_Implementation(const TArray<FCharData>& InCharacters)
{
    /* 로드 시점의 캐릭터 데이터를 Local GI에 보관합니다. 이 데이터는 월드 진입 직후 캐릭터 빙의 시 적용되며
    이후로는 비상시 사용되는 Defualt 데이터로만 사용됩니다. */
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->Characters = InCharacters;

	/* 캐릭터 목록 업데이트를 알립니다. */
	OnCharListUpdated.Broadcast();
}