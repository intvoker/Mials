// Fill out your copyright notice in the Description page of Project Settings.


#include "MialsCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AMialsCharacter::AMialsCharacter():
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(60.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMialsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = FollowCamera->FieldOfView;
	}
}

void AMialsCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0.f, Rotation.Yaw, 0.f};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction, Value);
	}
}

void AMialsCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0.f, Rotation.Yaw, 0.f};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
		AddMovementInput(Direction, Value);
	}
}

void AMialsCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMialsCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMialsCharacter::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	if (MuzzleFlash && !BarrelSocketName.IsNone())
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), BarrelSocketName);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage && !StartFireMontageSectionName.IsNone())
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(StartFireMontageSectionName);
	}

	const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(BarrelSocketName);

	if (!Socket)
		return;

	FTransform SocketTransform = Socket->GetSocketTransform(GetMesh());

	FVector BeamStart{SocketTransform.GetLocation()};
	FVector BeamEnd;

	if (!GetBeamEndLocation(BeamStart, BeamEnd))
		return;

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
	}

	if (BeamParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BeamStart);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
}

bool AMialsCharacter::GetBeamEndLocation(const FVector& BeamStartLocation, FVector& OutBeamEndLocation)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation{ViewportSize / 2.f};
	CrosshairLocation.Y -= 50.f;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool DeprojectResult = UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(),
	                                                                CrosshairLocation,
	                                                                CrosshairWorldPosition, CrosshairWorldDirection);

	if (!DeprojectResult)
		return false;

	FVector Start{CrosshairWorldPosition};
	FVector End{Start + CrosshairWorldDirection * 50000.f};

	FVector BeamStart{BeamStartLocation};
	FVector BeamEnd{End};

	FCollisionQueryParams CollisionQueryParams(NAME_None, true, this);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility,
	                                     CollisionQueryParams);
	if (HitResult.bBlockingHit)
	{
		//DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, true);

		FVector BeamDirection = HitResult.Location - BeamStart;
		BeamDirection.Normalize();
		BeamEnd = BeamStart + BeamDirection * 50000.f;
	}

	FHitResult BeamHitResult;
	GetWorld()->LineTraceSingleByChannel(BeamHitResult, BeamStart, BeamEnd, ECollisionChannel::ECC_Visibility,
	                                     CollisionQueryParams);
	if (BeamHitResult.bBlockingHit)
	{
		//DrawDebugLine(GetWorld(), BeamStart, BeamHitResult.Location, FColor::Green, true);

		BeamEnd = BeamHitResult.Location;
	}

	OutBeamEndLocation = BeamEnd;
	return true;
}

void AMialsCharacter::Aim()
{
	bAiming = true;
	FollowCamera->SetFieldOfView(CameraZoomedFOV);
}

void AMialsCharacter::StopAiming()
{
	bAiming = false;
	FollowCamera->SetFieldOfView(CameraDefaultFOV);
}

// Called every frame
void AMialsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMialsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMialsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMialsCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMialsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMialsCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AMialsCharacter::FireWeapon);

	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AMialsCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AMialsCharacter::StopAiming);
}
