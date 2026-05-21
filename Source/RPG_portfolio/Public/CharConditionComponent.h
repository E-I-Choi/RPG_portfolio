
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameTypes.h"
#include "CharConditionComponent.generated.h"

/** 
* @brief 3D 위젯 UI에 표시되는 Stat의 유형
*/
UENUM(BlueprintType)
enum class EUIStatType : uint8
{
	HP,
	MP,
	Level,
	Name
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConditionChanged, EUIStatType, StatType, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharDataChanged, EOnRepType, RepType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyCharDataChanged, EOnRepType, RepType);

/**
* @brief 3D 위젯으로 표시되어야 하는 캐릭터 변수들을 관리하는 액터.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PORTFOLIO_API UCharConditionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharConditionComponent();

	/** 
	* @brief 변수들에게 Rep 설정. 따로 호출 필요하지 않음.
	*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 
	* @brief Condition Component 구성요소에 해당하는 변수의 변화를 알리는 Delegate 
	*/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnConditionChanged OnConditionChanged;

	/** 
	* @brief 모든 캐릭터 데이터 변화를 알리는 Delegate
	*/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharDataChanged OnCharDataChanged;

	/**
	* @brief 오직 Local Pawn의 데이터 변화만을 알리는 Delegate 
	*/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMyCharDataChanged OnMyCharDataChanged;
	UPROPERTY(ReplicatedUsing = OnRep_CharName, BlueprintReadOnly, Category = "RPG/Character")
	FString CharName = TEXT("Name");
	UPROPERTY(ReplicatedUsing = OnRep_HP, BlueprintReadOnly, Category = "RPG/Character/UI")
	float HP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_MP, BlueprintReadOnly, Category = "RPG/Character/UI")
	float MP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_LV, BlueprintReadOnly, Category = "RPG/Character/UI")
	int32 Level = 1;

protected:

	UFUNCTION()
	void OnRep_CharName();
	UFUNCTION()
	void OnRep_HP();
	UFUNCTION()
	void OnRep_MP();
	UFUNCTION()
	void OnRep_LV();

	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
