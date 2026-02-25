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

	DOREPLIFETIME(UCharConditionComponent, HP);
	DOREPLIFETIME(UCharConditionComponent, MP);
	DOREPLIFETIME(UCharConditionComponent, Level);
}

void UCharConditionComponent::OnRep_HP()
{
	OnStatChanged.Broadcast(EUIStatType::HP, HP);
	OnCharDataChanged.Broadcast();
}

void UCharConditionComponent::OnRep_MP()
{
	OnStatChanged.Broadcast(EUIStatType::MP, MP);
	OnCharDataChanged.Broadcast();
}

void UCharConditionComponent::OnRep_LV()
{
	OnStatChanged.Broadcast(EUIStatType::Level, Level);
	OnCharDataChanged.Broadcast();
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

