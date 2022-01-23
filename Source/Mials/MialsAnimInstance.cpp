// Fill out your copyright notice in the Description page of Project Settings.


#include "MialsAnimInstance.h"
#include "MialsCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

		bIsInAir = MialsCharacter->GetCharacterMovement()->IsFalling();

		bIsAccelerating = MialsCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	}
}

void UMialsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MialsCharacter = Cast<AMialsCharacter>(TryGetPawnOwner());
}
