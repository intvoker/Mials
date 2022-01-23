// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MialsAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MIALS_API UMialsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere)
	class AMialsCharacter* MialsCharacter;

	UPROPERTY(VisibleAnywhere)
	float Speed;

	UPROPERTY(VisibleAnywhere)
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere)
	bool bIsAccelerating;
};
