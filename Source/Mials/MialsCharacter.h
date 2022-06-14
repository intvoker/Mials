// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MialsCharacter.generated.h"

UCLASS()
class MIALS_API AMialsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMialsCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void FireWeapon();

	bool GetBeamEndLocation(const FVector& BeamStartLocation, FVector& OutBeamEndLocation);

	void Aim();

	void StopAiming();

	void CameraZoomInterp(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsAiming() const
	{
		return bAiming;
	}

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere)
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	FName BarrelSocketName;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere)
	FName StartFireMontageSectionName;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere)
	bool bAiming;

	UPROPERTY(VisibleAnywhere)
	float CameraDefaultFOV;

	UPROPERTY(VisibleAnywhere)
	float CameraZoomedFOV;

	float CameraCurrentFOV;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed;
};
