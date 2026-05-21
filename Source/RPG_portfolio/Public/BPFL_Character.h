

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameTypes.h"
#include "BPFL_Character.generated.h"

/**
* @brief 케이스에 따라 캐릭터에게 필요한 수치를 구하는 함수 라이브러리
*/
UCLASS()
class RPG_PORTFOLIO_API UBPFL_Character : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* @brief 캐릭터의 기본 스탯 구하기
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character/Status")
	static FStatus GetBaseStatusByClass(EClassType InClassType);

	/** 
	* @brief 캐릭터의 기본 장비 구하기
	*/
	UFUNCTION(BlueprintCallable, Category = "RPG/Character/Status")
	static FEquips GetBaseEquipsByClass(EClassType InClassType);

};
