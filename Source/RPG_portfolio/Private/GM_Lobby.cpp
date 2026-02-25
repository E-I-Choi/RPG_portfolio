#include "GM_Lobby.h"
#include "PC_Lobby.h"

AGM_Lobby::AGM_Lobby()
{
}

void AGM_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APC_Lobby* LobbyPC = Cast<APC_Lobby>(NewPlayer);

	if (LobbyPC)
	{
		LobbyPC->Client_NotifyJoinedLobby();
		UE_LOG(LogTemp, Log, TEXT("Server: Player Joined. Sending Load Request..."));
	}

}
