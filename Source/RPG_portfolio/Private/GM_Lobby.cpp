

#include "GM_Lobby.h"
#include "RPGGameInstance.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "PC_Lobby.h"

AGM_Lobby::AGM_Lobby()
{
}

void AGM_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGM_Lobby::BeginPlay()
{
    Super::BeginPlay();
}

void AGM_Lobby::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    FHttpModule::Get().GetHttpManager().Tick(DeltaSeconds);
}
