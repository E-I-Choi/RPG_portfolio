#include "NetworkTask.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

#include "BPFL_Character.h"
#include "PC_Lobby.h"
#include "PC_RPG.h"

void UNetworkTask::InitUNetworkTask(TWeakObjectPtr<class APlayerController> Requestor, ENetConnectionType Type, FString InContext, float InTimeLimit, int32 InTicketId)
{
	Context = InContext;
	TimeLimit = InTimeLimit;
	TicketId = InTicketId;
	ConnectionType = Type;
	RequestorPC = Requestor;
}

void UNetworkTask::FinishTask(bool bSuccess)
{

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimeoutHandle);
	}

	if (OnPostTaskDestroy.IsBound())
	{
		OnPostTaskDestroy.Execute(this);
	}

	if (CurrentState == ETaskState::TimedOut) return;
	CurrentState = ETaskState::Finished;
	
}

void UNetworkTask::ExecuteTimer()
{
	CurrentState = ETaskState::InFlight;
	UWorld* World = GetWorld();
	if (!World) return;
	
	World->GetTimerManager().SetTimer(TimeoutHandle, [this]()
		{
			if (!IsValid(this)) { return; }
			CurrentState = ETaskState::TimedOut;
			FinishTask(false);
			UE_LOG(LogTemp, Warning, TEXT("Network doesn't answer: %s (Ticket: %d)"), *Context, TicketId);
			
		}, TimeLimit, false);



}

void UNetworkTask::ExecuteLoadCharacters(FString InEntityId, FString InEntityType)
{

	PlayFabEconomyPtr EconomyAPI = IPlayFabModuleInterface::Get().GetEconomyAPI();
	if (!EconomyAPI.IsValid()) return;

	ExecuteTimer();

	PlayFab::EconomyModels::FGetInventoryItemsRequest Request;

	Request.Entity = MakeShared<PlayFab::EconomyModels::FEntityKey>();
	Request.Entity->Id = InEntityId;
	Request.Entity->Type = InEntityType;

	Request.Filter = TEXT("ContentType eq 'Character'");

	EconomyAPI->GetInventoryItems(
		Request,
		PlayFab::UPlayFabEconomyAPI::FGetInventoryItemsDelegate::CreateLambda([this](const PlayFab::EconomyModels::FGetInventoryItemsResponse Result)
			{
				if (!(CurrentState == ETaskState::InFlight))
				{
					UE_LOG(LogTemp, Warning, TEXT("Zombie Task Defeated: Ignoring late response."));
					return;
				}

				TArray<FCharData> ExtractedCharacters;

				for (const auto& InvItem : Result.Items)
				{
					FCharData CharData;
					CharData.SetItemInstanceId(InvItem.Id);
					const TSharedPtr<FJsonObject>* OutObject;
					if (InvItem.DisplayProperties.GetJsonValue()->TryGetObject(OutObject))
					{
						TSharedPtr<FJsonObject> RootObj = InvItem.DisplayProperties.GetJsonValue()->AsObject();
						if (RootObj.IsValid())
						{
							FJsonObjectConverter::JsonObjectToUStruct(RootObj.ToSharedRef(), FCharData::StaticStruct(), &CharData);
						}
					}
					ExtractedCharacters.Add(CharData);
				}
				
				FNetworkReturnResult MyReturn;
				MyReturn.Response = ENetResponseType::Success;
				MyReturn.Type = ENetConnectionType::LoadCharacters;
				MyReturn.Context = FString::Printf(TEXT("Success: %d Characters loaded"), ExtractedCharacters.Num());

				if (this->RequestorPC.IsValid())
				{
					APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
					
					if (LobbyPC)
					{
						LobbyPC->ReceiveCharacterList(ExtractedCharacters);
						LobbyPC->ReceiveNetResponse(MyReturn);
					}
				}


				this->FinishTask(true);
			}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([this](const PlayFab::FPlayFabCppError& Error)
			{
				if (!(CurrentState == ETaskState::InFlight))
				{
					UE_LOG(LogTemp, Warning, TEXT("Zombie Task Defeated: Ignoring late response."));
					return;
				}
				FNetworkReturnResult MyReturn;
				MyReturn.Response = ENetResponseType::Failed;
				MyReturn.Type = ENetConnectionType::LoadCharacters;
				MyReturn.Context = Error.ErrorMessage;

				if (this->RequestorPC.IsValid())
				{
					APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
					if (LobbyPC)
					{
						LobbyPC->ReceiveNetResponse(MyReturn);
					}
				}

				this->FinishTask(false);
			})
	);
}

void UNetworkTask::ExecuteUpdateCharData(FString InEntityId, FString InEntityType, FCharData DataToSave)
{
	PlayFabEconomyPtr EconomyAPI = IPlayFabModuleInterface::Get().GetEconomyAPI();
	if (!EconomyAPI.IsValid()) return;

	ExecuteTimer();

	PlayFab::EconomyModels::FUpdateInventoryItemsRequest Request;

	Request.Entity = MakeShared<PlayFab::EconomyModels::FEntityKey>();
	Request.Entity->Id = InEntityId;
	Request.Entity->Type = InEntityType;

	TSharedRef<PlayFab::EconomyModels::FInventoryItem> ItemToUpdate = MakeShared<PlayFab::EconomyModels::FInventoryItem>();

	ItemToUpdate->Id = DataToSave.GetItemInstanceId();
	ItemToUpdate->Amount = 1;

	TSharedPtr<FJsonObject> RootObj = FJsonObjectConverter::UStructToJsonObject(DataToSave);

	if (RootObj.IsValid())
	{
		ItemToUpdate->DisplayProperties = MakeShared<FJsonValueObject>(RootObj);
	}

	Request.Item = ItemToUpdate; 

	EconomyAPI->UpdateInventoryItems(Request,
        PlayFab::UPlayFabEconomyAPI::FUpdateInventoryItemsDelegate::CreateLambda([this](const PlayFab::EconomyModels::FUpdateInventoryItemsResponse Result)
        {
			if (!(CurrentState == ETaskState::InFlight))
			{
				UE_LOG(LogTemp, Warning, TEXT("Zombie Task Defeated: Ignoring late response."));
				return;
			}

			FNetworkReturnResult MyReturn;
			MyReturn.Response = ENetResponseType::Success;
			MyReturn.Type = ENetConnectionType::SaveCharacters;
			MyReturn.Context = TEXT("Save Success!");

			if (this->RequestorPC.IsValid())
			{
				APC_RPG* RPGPC = Cast<APC_RPG>(RequestorPC);
				if (RPGPC)
				{
					RPGPC->ReceiveNetResponse(MyReturn);
				}
			}

            this->FinishTask(true);
        }),
        PlayFab::FPlayFabErrorDelegate::CreateLambda([this](const PlayFab::FPlayFabCppError& Error)
        {
			if (!(CurrentState == ETaskState::InFlight))
			{
					UE_LOG(LogTemp, Warning, TEXT("Zombie Task Defeated: Ignoring late response."));
					return;
			}
			FNetworkReturnResult MyReturn;
			MyReturn.Response = ENetResponseType::Failed;
			MyReturn.Type = ENetConnectionType::SaveCharacters;
			MyReturn.Context = Error.ErrorMessage;

			if (this->RequestorPC.IsValid())
			{
				APC_RPG* RPGPC = Cast<APC_RPG>(RequestorPC);
				if (RPGPC)
				{
					RPGPC->ReceiveNetResponse(MyReturn);
				}
			}
            this->FinishTask(false);
        })
    );
}

void UNetworkTask::ExecuteGrantNewCharItem(FString InEntityId, FString InEntityType, FString InName, EClassType InJob)
{
	PlayFabEconomyPtr EconomyAPI = IPlayFabModuleInterface::Get().GetEconomyAPI();
	if (!EconomyAPI.IsValid()) return;

	ExecuteTimer();

	PlayFab::EconomyModels::FAddInventoryItemsRequest Request;

	Request.Entity = MakeShared<PlayFab::EconomyModels::FEntityKey>();
	Request.Entity->Id = InEntityId;
	Request.Entity->Type = InEntityType;

	PlayFab::EconomyModels::FInventoryItem ItemPayload;
	ItemPayload.Id = TEXT("3495eae5-9b95-42c2-9b38-2d3d615286d1");
	ItemPayload.Amount = 1;

	FCharData InitialData;
	InitialData.Status = UBPFL_Character::GetBaseStatusByClass(InJob);
	InitialData.Equips = UBPFL_Character::GetBaseEquipsByClass(InJob);
	InitialData.Name = InName;
	InitialData.Job = InJob;

	TSharedPtr<FJsonObject> RootObj = FJsonObjectConverter::UStructToJsonObject(InitialData);
	if (RootObj.IsValid())
	{
		ItemPayload.DisplayProperties = MakeShared<FJsonValueObject>(RootObj);
	}
	

	EconomyAPI->AddInventoryItems(Request,
		PlayFab::UPlayFabEconomyAPI::FAddInventoryItemsDelegate::CreateLambda([this](const PlayFab::EconomyModels::FAddInventoryItemsResponse Result)
			{
				if (!(CurrentState == ETaskState::InFlight)) return;
				FNetworkReturnResult MyReturn;
				MyReturn.Response = ENetResponseType::Success;
				MyReturn.Type = ENetConnectionType::NewCharacter;

				if (this->RequestorPC.IsValid())
				{
					APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
					if (LobbyPC)
					{
						LobbyPC->ReceiveNetResponse(MyReturn);
					}
				}
				this->FinishTask(true);

			}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([this](const PlayFab::FPlayFabCppError& Error)
			{
				if (!(CurrentState == ETaskState::InFlight)) return;
				FNetworkReturnResult MyReturn;
				MyReturn.Response = ENetResponseType::Failed;
				MyReturn.Type = ENetConnectionType::NewCharacter;

				if (this->RequestorPC.IsValid())
				{
					APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
					if (LobbyPC)
					{
						LobbyPC->ReceiveNetResponse(MyReturn);
					}
				}
				this->FinishTask(false);
			})
	);
}