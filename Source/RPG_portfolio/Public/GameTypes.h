#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"

UENUM(BlueprintType)
enum class EClassType : uint8
{
	none UMETA(DisplayName = "None"),
	mage UMETA(DisplayName = "Mage"),
	archer UMETA(DisplayName = "Archer"),
	assassin UMETA(DisplayName = "Assassin"),
	warrior UMETA(DisplayName = "Warrior")
};

UENUM()
enum class EOnRepType : uint8
{
	CharName,
	Weapon,
	Hat,
	Top,
	Pants,
	Shoes,
	Job,
	Exp,
	Skills,
	HP,
	MP,
	Level,
	Status
};

UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	none  UMETA(DisplayName = "None"),
	cudgel UMETA(DisplayName = "Cudgel"),
	sword UMETA(DisplayName = "Sword"),
	wand UMETA(DisplayName = "Wand"),
	dagger UMETA(DisplayName = "Dagger"),
	bow UMETA(DisplayName = "Bow")
};

UENUM(BlueprintType)
enum class EHatType :uint8
{
	none UMETA(DisplayName = "None"),
	helmet UMETA(DisplayName = "Helmet"),
	hood UMETA(DisplayName = "Hood"),
	witch_hat UMETA(DisplayName = "Withch Hat"),
	feather_hat UMETA(DisplayName = "Feather Hat")
};

UENUM(BlueprintType)
enum class ETopType :uint8
{
	none UMETA(DisplayName = "None"),
	cotton_top UMETA(DisplayName = "Cotton Top"),
	armor_top UMETA(DisplayName = "Armor Top"),
	robe UMETA(DisplayName = "Robe")
};

UENUM(BlueprintType)
enum class EBottomsType :uint8
{
	none UMETA(DisplayName = "None"),
	cotton_bottoms UMETA(DisplayName = "Cotton Bottoms"),
	armor_bottoms UMETA(DisplayName = "Armor Bottoms")
};

UENUM(BlueprintType)
enum class EFootsType :uint8
{
	none UMETA(DisplayName = "None"),
	flat_shoes UMETA(DisplayName = "Flat Shoes"),
	combat_shoes UMETA(DisplayName = "Combat Shoes")
};

USTRUCT(BlueprintType)
struct FEquips
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Item")
	EWeaponType Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Item")
	EHatType Hat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Item")
	ETopType Top;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Item")
	EBottomsType Pants;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Item")
	EFootsType Shoes;

	FEquips()
		:Weapon(EWeaponType::none)
		,Hat(EHatType::none)
		,Top(ETopType::none)
		,Pants(EBottomsType::none)
		,Shoes(EFootsType::none)
	{}
};

USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character/Status")
	int32 Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character/Status")
	int32 Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character/Status")
	int32 Agility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character/Status")
	int32 Intelligence;

	FStatus()
		:Strength(0)
		,Health(0)
		,Agility(0)
		,Intelligence(0)
	{
	}
};
/*현재는 FCharData& Data 전체의 Dirty 여부만 판별하여 Data를 전부 업데이트 Or No 구조이지만
확장성을 고려하여 DirtyFlag는 각 멤버별로 Dirty를 판별할 수 있도록 지정해 둠*/
enum ECharacterDirtyFlags
{
	EDF_None = 0,
	EDF_Name = 1,
	EDF_Level = 1 << 1,
	EDF_Exp = 1 << 2,
	EDF_Equips = 1 << 3,
	EDF_Skills = 1 << 4,
	EDF_Job = 1 << 5,
	EDF_Status = 1 << 6,
	EDF_HP = 1 << 7,
	EDF_MP = 1<<8
};

USTRUCT(BlueprintType)
struct FCharData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	float Exp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	FEquips Equips;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	TArray<FString> Skills;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	EClassType Job;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	FStatus Status;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	float HP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/Character")
	float MP = 0.f;

	uint8 DirtyBits = 0;
	bool IsDirty(ECharacterDirtyFlags Flag) const { return  (DirtyBits & Flag) != 0 ; }
	void SetDirty(ECharacterDirtyFlags Flag) { DirtyBits |= Flag; }
	void ClearDirty() { DirtyBits = 0; }

	void SetItemInstanceId(FString InId) { ItemInstanceId = InId; }
	FString GetItemInstanceId() { return ItemInstanceId; }

private:
	UPROPERTY()
	FString ItemInstanceId = TEXT("");


public:
	FCharData()
		:Name(TEXT("None"))
		,Level(1)
	    ,Exp(0.0f)
	    ,Job(EClassType::none)
	{ }
};

UENUM(BlueprintType)
enum class EHudIntersectionType : uint8
{
	ShineCreate UMETA(DisplayName = "NewChar")
};

UENUM(BlueprintType)
enum class ENetConnectionType : uint8
{
	none UMETA(DisplayName = "None"),
	PlayFabLogIn UMETA(DisplayName = "PlayFabLogIn"),
	LoadCharacters UMETA(DisplayName = "LoadCharacters"),
	SaveCharacters UMETA(DisplayName = "SaveCharacters"),
	NewCharacter UMETA(DisplayName = "NewCharacter")
};

UENUM(BlueprintType)
enum class ENetResponseType : uint8
{
	Success UMETA(DisplayName = "Sucess"),
	Failed UMETA(DisplayName = "Failed"),
	Timeout UMETA(DisplayName = "Timeout")
};

USTRUCT(BlueprintType)
struct FNetworkReturnResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Network")
	ENetResponseType Response;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Network")
	ENetConnectionType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Network")
	FString Context;

	FNetworkReturnResult()
		:Response(ENetResponseType::Failed)
		,Type(ENetConnectionType::none)
		,Context(TEXT(""))
	{
	}
};