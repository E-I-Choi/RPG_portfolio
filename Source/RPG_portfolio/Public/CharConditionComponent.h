#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "CharConditionComponent.generated.h"


UENUM(BlueprintType)
enum class EUIStatType : uint8
{
	HP,
	MP,
	Level
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, EUIStatType, StatType, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharDataChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PORTFOLIO_API UCharConditionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharConditionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatChanged OnStatChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharDataChanged OnCharDataChanged;

	UPROPERTY(ReplicatedUsing = OnRep_HP, BlueprintReadOnly, Category = "RPG/Character/UI")
	float HP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_MP, BlueprintReadOnly, Category = "RPG/Character/UI")
	float MP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_LV, BlueprintReadOnly, Category = "RPG/Character/UI")
	int32 Level = 1;

	UFUNCTION()
	void OnRep_HP();
	UFUNCTION()
	void OnRep_MP();
	UFUNCTION()
	void OnRep_LV();


	void SetHP(float NewHP) { HP = NewHP;}
	void SetMP(float NewMP) { MP = NewMP;}
	void SetLevel(int32 NewLV) { Level = NewLV; }



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
