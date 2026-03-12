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
