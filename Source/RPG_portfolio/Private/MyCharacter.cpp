/* 인게임에서 실제 플레이어가 빙의하는 메인 캐릭터 */

#include "MyCharacter.h"

#include "CharConditionComponent.h"
#include "GameTypes.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"


AMyCharacter::AMyCharacter()
{
	ConditionComponent = CreateDefaultSubobject<UCharConditionComponent>(TEXT("ConditionComponent"));

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, Exp);
	DOREPLIFETIME(AMyCharacter, Weapon);
	DOREPLIFETIME(AMyCharacter, Hat);
	DOREPLIFETIME(AMyCharacter, Top);
	DOREPLIFETIME(AMyCharacter, Pants);
	DOREPLIFETIME(AMyCharacter, Shoes);
	DOREPLIFETIME(AMyCharacter, Job);
	DOREPLIFETIME(AMyCharacter, Skills);
	DOREPLIFETIME(AMyCharacter, Status);
}


void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::OnRep_CharExp()
{
	OnRepEvent(EOnRepType::Exp);
	if (ConditionComponent)
	{

		/* EXP 변경을 알립니다. */
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Exp);
	}
}


void AMyCharacter::OnRep_CharWeapon()
{
	OnRepEvent(EOnRepType::Weapon);
	if (ConditionComponent)
	{
		/* 무기 변경을 알립니다. */
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Weapon);
	}
}


void AMyCharacter::OnRep_CharHat()
{
	OnRepEvent(EOnRepType::Hat);
	if (ConditionComponent)
	{
		/* 모자 변경을 알립니다. */
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Hat);
	}
}


void AMyCharacter::OnRep_CharTop()
{
	OnRepEvent(EOnRepType::Top);
	if (ConditionComponent)
	{
		/* 상의 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Top);
	}
}


void AMyCharacter::OnRep_CharPants()
{
	OnRepEvent(EOnRepType::Pants);
	if (ConditionComponent)
	{
		/* 하의 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Pants);
	}
}


void AMyCharacter::OnRep_CharShoes()
{
	OnRepEvent(EOnRepType::Shoes);
	if (ConditionComponent)
	{
		/* 신발 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Shoes);
	}
}


void AMyCharacter::OnRep_CharJob()
{
	OnRepEvent(EOnRepType::Job);
	if (ConditionComponent)
	{
		/* 직업 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Job);
	}
}


void AMyCharacter::OnRep_CharSkills()
{
	OnRepEvent(EOnRepType::Skills);
	if (ConditionComponent)
	{
		/* 스킬 목록 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Skills);
	}
}


void AMyCharacter::OnRep_CharStatus()
{
	OnRepEvent(EOnRepType::Status);
	if (ConditionComponent)
	{
		/* 스탯 변경을 알립니다.*/
		ConditionComponent->OnCharDataChanged.Broadcast(EOnRepType::Status);
	}
}
