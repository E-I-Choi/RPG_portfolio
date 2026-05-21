

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameTypes.h"
#include "CharConditionComponent.h"
#include "MyCharacter.generated.h"


/** 
* @brief 인게임에서 실제 플레이어가 빙의하는 메인 캐릭터
*/
UCLASS()
class RPG_PORTFOLIO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	/** 
	* @brief 3D위젯에 표출되는 데이터를 관리하는 컴포넌트
	* 또한 로컬 위젯에게 데이터 변경을 알리는 역할
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharConditionComponent* ConditionComponent;

	/** 
	* @brief 변수들에게 Rep 설정. 따로 호출 필요하지 않음.*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	UPROPERTY(ReplicatedUsing = OnRep_CharExp, BlueprintReadOnly, Category = "RPG/Character")
	float Exp = 1.f;
	UPROPERTY(ReplicatedUsing = OnRep_CharWeapon, BlueprintReadOnly, Category = "RPG/Character")
	EWeaponType Weapon = EWeaponType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharHat, BlueprintReadOnly, Category = "RPG/Character")
	EHatType Hat = EHatType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharTop, BlueprintReadOnly, Category = "RPG/Character")
	ETopType Top = ETopType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharPants, BlueprintReadOnly, Category = "RPG/Character")
	EBottomsType Pants = EBottomsType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharShoes, BlueprintReadOnly, Category = "RPG/Character")
	EFootsType Shoes = EFootsType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharJob, BlueprintReadOnly, Category = "RPG/Character")
	EClassType Job = EClassType::none;
	UPROPERTY(ReplicatedUsing = OnRep_CharSkills, BlueprintReadOnly, Category = "RPG/Character")
	TArray<FString> Skills;
	UPROPERTY(ReplicatedUsing = OnRep_CharStatus, BlueprintReadOnly, Category = "RPG/Character")
	FStatus Status;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

protected:
	
	UFUNCTION()
	void OnRep_CharExp();
	UFUNCTION()
	void OnRep_CharWeapon();
	UFUNCTION()
	void OnRep_CharHat();
	UFUNCTION()
	void OnRep_CharTop();
	UFUNCTION()
	void OnRep_CharPants();
	UFUNCTION()
	void OnRep_CharShoes();
	UFUNCTION()
	void OnRep_CharJob();
	UFUNCTION()
	void OnRep_CharSkills();
	UFUNCTION()
	void OnRep_CharStatus();

	/**
	* @brief Condition을 제외한 모든 캐릭터 데이터 변경을 알립니다.
	* 이 이벤트는 장비·외형 등 비주얼 컴포넌트들의 업데이트 신호로 사용됩니다.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void OnRepEvent(EOnRepType RepType);
};
