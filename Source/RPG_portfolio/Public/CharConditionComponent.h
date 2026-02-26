#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameTypes.h"
#include "CharConditionComponent.generated.h"


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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PORTFOLIO_API UCharConditionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharConditionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnConditionChanged OnConditionChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharDataChanged OnCharDataChanged;
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
	UFUNCTION()
	void BroadcastMyPawnChangeWithValidation(EOnRepType Type);

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
