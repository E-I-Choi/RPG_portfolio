// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_RPG.h"
#include "RPGGameInstance.h"

AGM_RPG::AGM_RPG()
{
}

void AGM_RPG::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(SaveTimerHandle, this, &AGM_RPG::SaveAllDirtyPlayerData, 300.0f, true);

}

void AGM_RPG::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGM_RPG::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
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
				GI->CharDataDepot.Remove(PC);
			}
		}
	}
}
