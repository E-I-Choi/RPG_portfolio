#include "GM_Lobby.h"
#include "RPGGameInstance.h"
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
		UE_LOG(LogTemp, Log, TEXT("Server: Player Joined. Sending Load Request..."));
		URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
		GI->LoadAllCharactersFromServer(LobbyPC);
	}

}
