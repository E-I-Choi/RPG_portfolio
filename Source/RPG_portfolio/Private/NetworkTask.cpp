#include "NetworkTask.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

#include "PlayFabServerAPI.h"
#include "PlayFabEconomyAPI.h"

#include "BPFL_Character.h"
#include "PC_Lobby.h"
#include "PC_RPG.h"

void ANetworkTask::InitUNetworkTask(TWeakObjectPtr<class APlayerController> Requestor, ENetConnectionType Type, FString InContext, float InTimeLimit, int32 InTicketId)
{
	Context = InContext;
	TimeLimit = InTimeLimit;
	TicketId = InTicketId;
	ConnectionType = Type;
	RequestorPC = Requestor;
}

void ANetworkTask::FinishTask(bool bSuccess)
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

void ANetworkTask::OnSucessLoadCharacters(FServerGetUserInventoryResult Result, UObject* CustomData)
{
	UE_LOG(LogTemp, Warning, TEXT("=== PlayFab API Response Received! ==="));
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

void ANetworkTask::OnSucessGrantCharacter(FServerGrantItemsToUserResult Result, UObject* CustomData)
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

void ANetworkTask::OnSucessUpdateCharacter(FServerEmptyResponse Result, UObject* CustomData)
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

void ANetworkTask::OnLoadFailure(FPlayFabError Error, UObject* CustomData)
{
	UE_LOG(LogTemp, Warning, TEXT("=== PlayFab API Fail Response Received! ==="));
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

void ANetworkTask::OnGrantFailure(FPlayFabError Error, UObject* CustomData)
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

void ANetworkTask::OnUpdateFailure(FPlayFabError Error, UObject* CustomData)
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

void ANetworkTask::ExecuteTimer()
{
	CurrentState = ETaskState::InFlight;
	UWorld* World = GetWorld();
	if (!World) 
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no world)"));
		return;
	}
	
	World->GetTimerManager().SetTimer(TimeoutHandle, [this]()
		{
			if (!IsValid(this)) { return; }
			CurrentState = ETaskState::TimedOut;
			UE_LOG(LogTemp, Warning, TEXT("Network doesn't answer: %s (Ticket: %d)"), *Context, TicketId);
			FinishTask(false);
			
			
		}, TimeLimit, false);



}

void ANetworkTask::ExecuteLoadCharacters(FString InCustomId)
{
	ExecuteTimer();

	FServerGetUserInventoryRequest Request;
	Request.PlayFabId = InCustomId;

	UPlayFabServerAPI::FDelegateOnSuccessGetUserInventory SuccessDelegate;
	SuccessDelegate.BindUFunction(this, FName("OnSucessLoadCharacters"));

	UPlayFabServerAPI::FDelegateOnFailurePlayFabError FailureDelegate;
	FailureDelegate.BindUFunction(this, FName("OnLoadFailure"));

	FString CurrentTitleId = GetDefault<UPlayFabRuntimeSettings>()->TitleId;

	auto Settings = GetDefault<UPlayFabRuntimeSettings>();

	if (FHttpModule::Get().IsHttpEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Http is disabled"));
	}

	UPlayFabServerAPI* Task = UPlayFabServerAPI::GetUserInventory(
		Request, SuccessDelegate, FailureDelegate, nullptr);
	Task->Activate();
	
}

void ANetworkTask::ExecuteUpdateCharData(FString InCustomId, FCharData DataToSave)
{
	
	FServerUpdateUserInventoryItemDataRequest Request;
	ExecuteTimer();

	Request.PlayFabId = InCustomId;
	Request.ItemInstanceId = DataToSave.GetItemInstanceId();

	UPlayFabJsonObject* ItemToUpdate = NewObject<UPlayFabJsonObject>();

	if (DataToSave.IsDirty(EDF_Name))
	{
		ItemToUpdate->SetStringField(TEXT("Name"), DataToSave.Name);
	}
	if (DataToSave.IsDirty(EDF_Level))
	{
		ItemToUpdate->SetNumberField(TEXT("Level"), DataToSave.Level);
	}
	if (DataToSave.IsDirty(EDF_Exp))
	{
		ItemToUpdate->SetNumberField(TEXT("Exp"), DataToSave.Exp);
	}
	if (DataToSave.IsDirty(EDF_HP))
	{
		ItemToUpdate->SetNumberField(TEXT("HP"), DataToSave.HP);
	}
	if (DataToSave.IsDirty(EDF_MP))
	{
		ItemToUpdate->SetNumberField(TEXT("MP"), DataToSave.MP);
	}
	if (DataToSave.IsDirty(EDF_Job))
	{
		UEnum* JobEnum = StaticEnum<EClassType>();
		if (JobEnum)
		{
			FString JobString = JobEnum->GetNameStringByValue((int64)DataToSave.Job);
			ItemToUpdate->SetStringField(TEXT("Job"), JobString);
		}
	}
	if (DataToSave.IsDirty(EDF_Equips))
	{
		FString EquipsJson;
		if (FJsonObjectConverter::UStructToJsonObjectString(DataToSave.Equips, EquipsJson))
		{
			UPlayFabJsonObject* EquipsJsonObj = NewObject<UPlayFabJsonObject>();
			EquipsJsonObj->DecodeJson(EquipsJson);
			ItemToUpdate->SetObjectField(TEXT("Equips"), EquipsJsonObj);
		}
	}
	if (DataToSave.IsDirty(EDF_Status))
	{
		FString StatusJson;
		if (FJsonObjectConverter::UStructToJsonObjectString(DataToSave.Status, StatusJson))
		{
			UPlayFabJsonObject* StatusJsonObj = NewObject<UPlayFabJsonObject>();
			StatusJsonObj->DecodeJson(StatusJson);
			ItemToUpdate->SetObjectField(TEXT("Status"), StatusJsonObj);
		}
	}
	if (DataToSave.IsDirty(EDF_Skills))
	{
		ItemToUpdate->SetStringArrayField(TEXT("Skills"), DataToSave.Skills);
	}

	ItemToUpdate->SetNumberField(TEXT("LocX"), DataToSave.LocX);
	ItemToUpdate->SetNumberField(TEXT("LocY"), DataToSave.LocY);
	ItemToUpdate->SetNumberField(TEXT("LocZ"), DataToSave.LocZ);
	

	Request.Data = ItemToUpdate;

	UPlayFabServerAPI::FDelegateOnSuccessUpdateUserInventoryItemCustomData SuccessDelegate;
	SuccessDelegate.BindUFunction(this, FName("OnSucessUpdateCharacter"));

	UPlayFabServerAPI::FDelegateOnFailurePlayFabError FailureDelegate;
	FailureDelegate.BindUFunction(this, FName("OnUpdateFailure"));

	UPlayFabServerAPI::UpdateUserInventoryItemCustomData(
		Request,
		SuccessDelegate,
		FailureDelegate,
		nullptr
	);
}

void ANetworkTask::ExecuteGrantNewCharItem(FString InCustomId, FString InName, EClassType InJob)
{

	FEconomyAddInventoryItemsRequest Request;
	ExecuteTimer();

	UPlayFabJsonObject* EntityObj = NewObject<UPlayFabJsonObject>();
	EntityObj->SetStringField(TEXT("Id"), InCustomId);
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