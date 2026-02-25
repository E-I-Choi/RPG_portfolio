// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameTypes.h"
#include "HUD_RPG.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PORTFOLIO_API AHUD_RPG : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG/HUD")
	TMap<EHudIntersectionType, bool> Intersection = {
		{EHudIntersectionType::ShineCreate, true}
	};
	
};
