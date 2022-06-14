// Fill out your copyright notice in the Description page of Project Settings.


#include "MialsAnimInstance.h"
#include "MialsCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMialsAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MialsCharacter == nullptr)
	{
		MialsCharacter = Cast<AMialsCharacter>(TryGetPawnOwner());
	}

	if (MialsCharacter)
	{
		FVector Velocity = MialsCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bInAir = MialsCharacter->GetCharacterMovement()->IsFalling();

		bAccelerating = MialsCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

		FRotator AimRotation = MialsCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MialsCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (Speed > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = MialsCharacter->IsAiming();

		/*
		FString AimRotationMessage = FString::Printf(TEXT("Aim Rotation: %f"), AimRotation.Yaw);
		FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
		FString MovementOffsetYawMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);
		FString LastMovementOffsetYawMessage = FString::Printf(TEXT("Last Movement Offset Yaw: %f"), LastMovementOffsetYaw);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::White, AimRotationMessage, false);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::White, MovementRotationMessage, false);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::White, MovementOffsetYawMessage, false);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::White, LastMovementOffsetYawMessage, false);
		}
		*/
	}
}

void UMialsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MialsCharacter = Cast<AMialsCharacter>(TryGetPawnOwner());
}
