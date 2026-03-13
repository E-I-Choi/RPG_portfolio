


#include "PC_Lobby.h"
#include "RPGGameInstance.h"




bool APC_Lobby::Server_ReqCreateNewChar_Validate(const FString& InEntityId, const EClassType& InJob, const FString& InName)
{
	return true;
}

void APC_Lobby::Server_ReqCreateNewChar_Implementation(const FString& InEntityId, const EClassType& InJob, const FString& InName)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->CreateNewCharacter(this, InEntityId, InJob, InName);
}

bool APC_Lobby::Server_ReqLoadAllCharacters_Validate(const FString& InEntityId)
{
	return true;
}

void APC_Lobby::Server_ReqLoadAllCharacters_Implementation(const FString& InEntityId)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->LoadAllCharactersFromServer(this, InEntityId);
}

bool APC_Lobby::Server_ReqDeleteCharacter_Validate()
{
	return true;
}

void APC_Lobby::Server_ReqDeleteCharacter_Implementation()
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	//시간이 지체된 관계로 Delete Character 기능 생략
}



void APC_Lobby::Client_ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result)
{

	switch (Result.Type)
	{
	default: break;
	case ENetConnectionType::NewCharacter:
		if (Result.Response == ENetResponseType::Success)
		{
			URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
			Server_ReqLoadAllCharacters(GI->GetEntityId());
		}
		break;
	}

	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->OnSystemMessageLog.Broadcast(Result.Context, GI->GetSeverity(Result));
}


void APC_Lobby::Client_ReceiveCharacterList_Implementation(const TArray<FCharData>& InCharacters)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->Characters = InCharacters;
	OnCharListUpdated.Broadcast();
}
