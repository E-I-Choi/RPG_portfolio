#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameTypes.h"
#include "CharConditionComponent.h"
#include "MyCharacter.generated.h"

class UMyConditionComponent;

UCLASS()
class RPG_PORTFOLIO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharConditionComponent* ConditionComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	UPROPERTY(ReplicatedUsing = OnRep_CharName, BlueprintReadOnly , Category = "RPG/Character")
	FString CharName = TEXT("Name");
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
	void OnRep_CharName();
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
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void OnRepEvent(EOnRepType RepType);
};
