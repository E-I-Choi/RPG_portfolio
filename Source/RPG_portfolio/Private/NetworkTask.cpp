#include "NetworkTask.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

#include "PlayFabServerAPI.h"
#include "PlayFabEconomyAPI.h"

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

void UNetworkTask::OnSucessLoadCharacters(FServerGetUserInventoryResult Result, UObject* CustomData)
{
	if (!(CurrentState == ETaskState::InFlight))
	{
		UE_LOG(LogTemp, Warning, TEXT("Zombie Task Defeated: Ignoring late response."));
		return;
	}


	TArray<FCharData> ExtractedCharacters;

	for (const auto& InvItem : Result.Inventory)
	{
		if (InvItem->GetStringField(TEXT("ItemClass")) == TEXT("Character"))
		{
			FCharData CharData;
			CharData.SetItemInstanceId(InvItem->GetStringField(TEXT("ItemInstanceId")));

			UPlayFabJsonObject* CustomDataObj = InvItem->GetObjectField(TEXT("CustomData"));

			if (CustomDataObj != nullptr)
			{
				CharData.Name = CustomDataObj->GetStringField(TEXT("Name"));
				CharData.Level = CustomDataObj->GetNumberField(TEXT("Level"));
				CharData.Exp = CustomDataObj->GetNumberField(TEXT("Exp"));
				CharData.HP = CustomDataObj->GetNumberField(TEXT("HP"));
				CharData.MP = CustomDataObj->GetNumberField(TEXT("MP"));

				FString JobString = CustomDataObj->GetStringField(TEXT("Job"));

				UEnum* JobEnum = StaticEnum<EClassType>();
				if (JobEnum)
				{
					int32 JobValue = JobEnum->GetIndexByName(FName(*JobString));

					if (JobValue != INDEX_NONE)
					{
						CharData.Job = static_cast<EClassType>(JobValue);
					}
					else
					{
						CharData.Job = EClassType::none;
					}
				}

				UPlayFabJsonObject* EquipsObj = CustomDataObj->GetObjectField(TEXT("Equips"));
				if (EquipsObj) {
					FJsonObjectConverter::JsonObjectToUStruct(EquipsObj->GetRootObject().ToSharedRef(), &CharData.Equips);
				}

				UPlayFabJsonObject* StatusObj = CustomDataObj->GetObjectField(TEXT("Status"));
				if (StatusObj) {
					FJsonObjectConverter::JsonObjectToUStruct(StatusObj->GetRootObject().ToSharedRef(), &CharData.Status);
				}

				CharData.Skills = CustomDataObj->GetStringArrayField(TEXT("Skills"));
			}

			ExtractedCharacters.Add(CharData);
		}
	}

	FNetworkReturnResult MyReturn;
	MyReturn.Response = ENetResponseType::Success;
	MyReturn.Type = ENetConnectionType::LoadCharacters;
	MyReturn.Context = FString::Printf(TEXT("Success: %d Characters loaded"), ExtractedCharacters.Num());

	if (this->RequestorPC.IsValid())
	{
		APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);

		if (LobbyPC && LobbyPC->GetNetConnection() != nullptr)
		{
			LobbyPC->Client_ReceiveCharacterList(ExtractedCharacters);
			LobbyPC->Client_ReceiveNetResponse(MyReturn);
		}
	}


	this->FinishTask(true);
}

void UNetworkTask::OnSucessGrantCharacter(FServerGrantItemsToUserResult Result, UObject* CustomData)
{
	if (!(CurrentState == ETaskState::InFlight)) return;
	FNetworkReturnResult MyReturn;
	MyReturn.Response = ENetResponseType::Success;
	MyReturn.Type = ENetConnectionType::NewCharacter;

	if (this->RequestorPC.IsValid())
	{
		APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
		if (LobbyPC && LobbyPC->GetNetConnection() != nullptr)
		{
			LobbyPC->Client_ReceiveNetResponse(MyReturn);
		}
	}
	this->FinishTask(true);
}

void UNetworkTask::OnSucessUpdateCharacter(FEconomyUpdateInventoryItemsResponse Result, UObject* CustomData)
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
		if (RPGPC && RPGPC->GetNetConnection() != nullptr)
		{
			RPGPC->Client_ReceiveNetResponse(MyReturn);
		}
	}

	this->FinishTask(true);
}

void UNetworkTask::OnLoadFailure(FPlayFabError Error, UObject* CustomData)
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
		if (LobbyPC && LobbyPC->GetNetConnection() != nullptr)
		{
			LobbyPC->Client_ReceiveNetResponse(MyReturn);
		}
	}

	this->FinishTask(false);
}

void UNetworkTask::OnGrantFailure(FPlayFabError Error, UObject* CustomData)
{
	if (!(CurrentState == ETaskState::InFlight)) return;
	FNetworkReturnResult MyReturn;
	MyReturn.Response = ENetResponseType::Failed;
	MyReturn.Type = ENetConnectionType::NewCharacter;
	MyReturn.Context = Error.ErrorMessage;

	if (this->RequestorPC.IsValid())
	{
		APC_Lobby* LobbyPC = Cast<APC_Lobby>(RequestorPC);
		if (LobbyPC && LobbyPC->GetNetConnection() != nullptr)
		{
			LobbyPC->Client_ReceiveNetResponse(MyReturn);
		}
	}
	this->FinishTask(false);
}

void UNetworkTask::OnUpdateFailure(FPlayFabError Error, UObject* CustomData)
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
				if (RPGPC && RPGPC->GetNetConnection() != nullptr)
				{
					RPGPC->Client_ReceiveNetResponse(MyReturn);
				}
			}
			this->FinishTask(false);
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

void UNetworkTask::ExecuteLoadCharacters(FString InCustomId)
{
	ExecuteTimer();
	FServerGetUserInventoryRequest Request;
	Request.PlayFabId = InCustomId;

	UPlayFabServerAPI::FDelegateOnSuccessGetUserInventory SuccessDelegate;
	SuccessDelegate.BindUFunction(this, FName("OnSucessLoadCharacters"));

	UPlayFabServerAPI::FDelegateOnFailurePlayFabError FailureDelegate;
	FailureDelegate.BindUFunction(this, FName("OnLoadFailure"));

	UPlayFabServerAPI::GetUserInventory(
		Request,
		SuccessDelegate,
		FailureDelegate,
		nullptr
	);
}

void UNetworkTask::ExecuteUpdateCharData(FString InCustomId, FCharData DataToSave)
{
	
	FServerUpdateUserInventoryItemDataRequest Request;
	ExecuteTimer();

	Request.PlayFabId = InCustomId;

	UPlayFabJsonObject* ItemToUpdate = NewObject<UPlayFabJsonObject>();

	TSharedPtr<FJsonObject> RootJsonObj = FJsonObjectConverter::UStructToJsonObject(DataToSave);

	if (RootJsonObj.IsValid())
	{
		UPlayFabJsonObject* DisplayPropsObj = NewObject<UPlayFabJsonObject>();
		DisplayPropsObj->SetRootObject(RootJsonObj);
		ItemToUpdate->SetObjectField(TEXT("DisplayProperties"), DisplayPropsObj);
	}
	
	ItemToUpdate->SetStringField(TEXT("Id"), DataToSave.GetItemInstanceId());
	Request.ItemInstanceId = DataToSave.GetItemInstanceId();
	Request.Data = ItemToUpdate;


	UPlayFabEconomyAPI::FDelegateOnSuccessUpdateInventoryItems SuccessDelegate;
	SuccessDelegate.BindUFunction(this, FName("OnSucessUpdateCharacter"));

	UPlayFabEconomyAPI::FDelegateOnFailurePlayFabError FailureDelegate;
	FailureDelegate.BindUFunction(this, FName("OnUpdateFailure"));

	UPlayFabServerAPI* Proxy = UPlayFabEconomyAPI::UpdateInventoryItems(
		Request,
		SuccessDelegate,
		FailureDelegate,
		nullptr
	);
}

void UNetworkTask::ExecuteGrantNewCharItem(FString InEntityId, FString InName, EClassType InJob)
{

	FEconomyAddInventoryItemsRequest Request;
	ExecuteTimer();

	UPlayFabJsonObject* EntityObj = NewObject<UPlayFabJsonObject>();
	EntityObj->SetStringField(TEXT("Id"), InEntityId);
	EntityObj->SetStringField(TEXT("Type"), TEXT("title_player_account"));

	Request.Entity = EntityObj;

	UPlayFabJsonObject* ItemReference = NewObject<UPlayFabJsonObject>();
	
	ItemReference->SetStringField(TEXT("Id"), TEXT("3495eae5-9b95-42c2-9b38-2d3d615286d1"));
	ItemReference->SetNumberField(TEXT("Amount"), 1);

	FCharData InitialData;
	InitialData.Status = UBPFL_Character::GetBaseStatusByClass(InJob);
	InitialData.Equips = UBPFL_Character::GetBaseEquipsByClass(InJob);
	InitialData.Name = InName;
	InitialData.Job = InJob;

	TSharedPtr<FJsonObject> RootObj = FJsonObjectConverter::UStructToJsonObject(InitialData);
	if (RootObj.IsValid())
	{
		UPlayFabJsonObject* DisplayPropsObj = NewObject<UPlayFabJsonObject>();
		DisplayPropsObj->SetRootObject(RootObj);
		ItemReference->SetObjectField(TEXT("DisplayProperties"), DisplayPropsObj);
	}
	Request.Item = ItemReference;
	
	UPlayFabEconomyAPI::FDelegateOnSuccessAddInventoryItems SuccessDelegate;
	SuccessDelegate.BindUFunction(this, FName("OnSucessGrantCharacter"));

	UPlayFabEconomyAPI::FDelegateOnFailurePlayFabError FailureDelegate;
	FailureDelegate.BindUFunction(this, FName("OnGrantFailure"));

	UPlayFabEconomyAPI* Proxy = UPlayFabEconomyAPI::AddInventoryItems(
		Request,
		SuccessDelegate,
		FailureDelegate,
		nullptr
	);
}