


#include "PC_Lobby.h"
#include "RPGGameInstance.h"




void APC_Lobby::ReceiveNetResponse_Implementation(const FNetworkReturnResult& Result)
{
	switch (Result.Type)
	{
	default: break;
	case ENetConnectionType::SaveCharacters:
		return;
	case ENetConnectionType::LoadCharacters:
		return;

	}
}

void APC_Lobby::ReceiveCharacterList_Implementation(const TArray<FCharData>& InCharacters)
{
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	GI->Characters = InCharacters;
	OnCharListUpdated.Broadcast();
}

void APC_Lobby::Client_NotifyJoinedLobby_Implementation() 
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->LoadAllCharactersFromServer(this);
	}
	OnJoinedLobbyServer();
}

void APC_Lobby::OnJoinedLobbyServer_Implementation()
{
}


