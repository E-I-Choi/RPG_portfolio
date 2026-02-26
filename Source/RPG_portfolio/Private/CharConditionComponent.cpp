// Fill out your copyright notice in the Description page of Project Settings.


#include "CharConditionComponent.h"

// Sets default values for this component's properties
UCharConditionComponent::UCharConditionComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharConditionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharConditionComponent, CharName);
	DOREPLIFETIME(UCharConditionComponent, HP);
	DOREPLIFETIME(UCharConditionComponent, MP);
	DOREPLIFETIME(UCharConditionComponent, Level);
}

void UCharConditionComponent::OnRep_CharName()
{
	OnConditionChanged.Broadcast(EUIStatType::Name, 0);
	OnCharDataChanged.Broadcast(EOnRepType::CharName);
}

void UCharConditionComponent::OnRep_HP()
{
	OnConditionChanged.Broadcast(EUIStatType::HP, HP);
	OnCharDataChanged.Broadcast(EOnRepType::HP);
}

void UCharConditionComponent::OnRep_MP()
{
	OnConditionChanged.Broadcast(EUIStatType::MP, MP);
	OnCharDataChanged.Broadcast(EOnRepType::MP);
}

void UCharConditionComponent::OnRep_LV()
{
	OnConditionChanged.Broadcast(EUIStatType::Level, Level);
	OnCharDataChanged.Broadcast(EOnRepType::Level);
}

void UCharConditionComponent::BroadcastMyPawnChangeWithValidation(EOnRepType Type)
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (OwnerPawn && OwnerPawn->IsLocallyControlled())
		{
			OnMyCharDataChanged.Broadcast(Type);
		}
	}
}


// Called when the game starts
void UCharConditionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCharConditionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

