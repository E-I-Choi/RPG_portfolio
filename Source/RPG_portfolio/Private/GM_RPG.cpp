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
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	FCharData UserCharData = Cast<URPGGameInstance>(NewPlayer->GetGameInstance())->GetCurrentChar();
	GI->RegisterPlayer(NewPlayer, UserCharData.GetItemInstanceId());
}

void AGM_RPG::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	URPGGameInstance* GI = Cast<URPGGameInstance>(GetGameInstance());
	FCharData UserCharData = Cast<URPGGameInstance>(Exiting->GetGameInstance())->GetCurrentChar();
	APlayerController* PC = Cast<APlayerController>(Exiting);

	if (PC)
	{
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

		if (Data.DirtyBits != 0)
		{
			GI->SaveCharacterToServer(PC, Data);
		}
	}
}
