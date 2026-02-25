// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "GameTypes.h"
#include "Net/UnrealNetwork.h"

#include "RPGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharNameChanged, FName, NewName);


UCLASS()
class RPG_PORTFOLIO_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ARPGPlayerState();

	
};
