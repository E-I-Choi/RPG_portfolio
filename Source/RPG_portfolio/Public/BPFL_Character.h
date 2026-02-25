#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameTypes.h"
#include "BPFL_Character.generated.h"


UCLASS()
class RPG_PORTFOLIO_API UBPFL_Character : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RPG/Character/Status")
	static FStatus GetBaseStatusByClass(EClassType InClassType);
	UFUNCTION(BlueprintCallable, Category = "RPG/Character/Status")
	static FEquips GetBaseEquipsByClass(EClassType InClassType);

};
