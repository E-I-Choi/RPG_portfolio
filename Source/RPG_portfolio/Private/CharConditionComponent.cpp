#include "CharConditionComponent.h"


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
	/* 이름 변경을 알립니다. */
	OnConditionChanged.Broadcast(EUIStatType::Name, 0);
	OnCharDataChanged.Broadcast(EOnRepType::CharName);
}



void UCharConditionComponent::OnRep_HP()
{
	/* HP 변경을 알립니다. */
	OnConditionChanged.Broadcast(EUIStatType::HP, HP);
	OnCharDataChanged.Broadcast(EOnRepType::HP);
}


void UCharConditionComponent::OnRep_MP()
{
	/* MP 변경을 알립니다. */
	OnConditionChanged.Broadcast(EUIStatType::MP, MP);
	OnCharDataChanged.Broadcast(EOnRepType::MP);
}


void UCharConditionComponent::OnRep_LV()
{
	/* 레벨 변경을 알립니다. */
	OnConditionChanged.Broadcast(EUIStatType::Level, Level);
	OnCharDataChanged.Broadcast(EOnRepType::Level);
}

void UCharConditionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCharConditionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

