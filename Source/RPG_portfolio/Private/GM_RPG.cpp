


#include "GM_RPG.h"
#include "RPGGameInstance.h"

AGM_RPG::AGM_RPG()
{
}


void AGM_RPG::BeginPlay()
{
	Super::BeginPlay();

	/* 300초마다 모든 플레이어의 Dirty데이터를 DB에게 동기화 */
	GetWorldTimerManager().SetTimer(SaveTimerHandle, this, &AGM_RPG::SaveAllDirtyPlayerData, 300.0f, true);

}

void AGM_RPG::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	APlayerController* PC = Cast<APlayerController>(NewPlayer);

	if (PC)
	{
		APC_RPG* ThePC = Cast<APC_RPG>(PC);
		if (ThePC)
		{
			ThePC->Client_ReceiveReqRegisterPlayer();
		}
	}

}


void AGM_RPG::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	/* 플레이어가 로그아웃하면 플레이어 데이터를 DB에 동기화 요청하고 관리 목록에서 플레이어 PC 제거 */
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	APlayerController* PC = Cast<APlayerController>(Exiting);

	if (PC)
	{
		APC_RPG* ThePC = Cast<APC_RPG>(PC);
		if (ThePC)
		{
			ThePC->Client_ReceiveReqSaveCharacter();
		}
		GI->CharDataDepot.Remove(PC);
	}
}


void AGM_RPG::SaveAllDirtyPlayerData()
{

	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	if (!GI) return;

	UE_LOG(LogTemp, Log, TEXT("--- Periodic Save Started ---"));

	for (auto& Elem : GI->CharDataDepot)
	{
		APlayerController* PC = Elem.Key;
		FCharData& Data = Elem.Value;
		if (!PC)
		{
			APC_RPG* ThePC = Cast<APC_RPG>(PC);
			if (ThePC)
			{
				ThePC->Client_ReceiveReqSaveCharacterWithData(Data);
				Data.ClearDirty();
			}
		}
	}
}
