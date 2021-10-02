// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudumDare49Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

//////////////////////////////////////////////////////////////////////////
// ALudumDare49Character

ALudumDare49Character::ALudumDare49Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//Set Default Locked On State to false
	LockedOn = false;

	//Set Default Jump Height
	JumpHeight = 600.0f;

	//default sprinting
	sprinting = false;

}

//////////////////////////////////////////////////////////////////////////
// Input

float ALudumDare49Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, 
	AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (health > 0.0f)
	{
		health -= Damage;
		if (health <= 0.0f)

		return Damage;
	}
	return 0.0f;
}

void ALudumDare49Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALudumDare49Character::DoubleJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALudumDare49Character::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALudumDare49Character::StopSprint);

	PlayerInputComponent->BindAction("LockOn", IE_Released, this, &ALudumDare49Character::ToggleLockOn);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALudumDare49Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALudumDare49Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALudumDare49Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALudumDare49Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ALudumDare49Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ALudumDare49Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALudumDare49Character::OnResetVR);
}


void ALudumDare49Character::OnResetVR()
{
	// If LudumDare49 is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in LudumDare49.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ALudumDare49Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ALudumDare49Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ALudumDare49Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALudumDare49Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALudumDare49Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		if (LockedOn)
		{
			Direction = FollowCamera->GetForwardVector();
		}	
		AddMovementInput(Direction, Value);
	}
}

void ALudumDare49Character::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//if we are locked on
		if (LockedOn)
		{
			Direction = FollowCamera->GetRightVector();
		}
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ALudumDare49Character::ToggleLockOn()
{
	if (Controller != nullptr)
	{
		LockedOn = !LockedOn;
		if (!LockedOn)
		{
			CameraBoom->bUsePawnControlRotation = true;
			CameraBoom->bInheritPitch = true;
			CameraBoom->bInheritYaw = true;
			CameraBoom->bInheritRoll = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			return;
		}

		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritYaw = true;
		CameraBoom->bInheritRoll = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		if (sprinting)
		{
			StopSprint();
		}

		// if we haven't detected the boss yet, do it here
		if (Boss == nullptr)
		{
			TObjectPtr<AActor> actor = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
			if (actor != nullptr && actor->IsA(ABossCharacter::StaticClass()))
			{
				Boss = Cast<ABossCharacter>(actor);
			}
		}
	}
}

void ALudumDare49Character::Tick(float deltaTime)
{
	if (LockedOn && Controller != nullptr && Boss != nullptr)
	{
		FVector playerPosition = GetActorLocation();
		FVector bossPosition = Boss->GetActorLocation();
		FVector flat = FVector(0.0f);
		SetActorRotation(FRotationMatrix::MakeFromX(bossPosition - playerPosition).Rotator());
		FRotator lockedRotation = FRotator(0.0f, GetActorQuat().Rotator().Yaw, GetActorQuat().Rotator().Roll);
		SetActorRotation(lockedRotation);
	}
	if (sprinting)
	{
		if (stamina > 0.0f) {
			stamina -= 1.2f;
		}
		else
		{
			StopSprint();
		}
	}
	if (!sprinting && !dodging)
	{
		if (stamina < 100.0f)
		{
			stamina += 0.8f;
		}
	}
	Super::Tick(deltaTime);
}

void ALudumDare49Character::DoubleJump()
{
	if (Controller != nullptr && doubleJt <= 1)
	{
		float velX = (doubleJt < 1) ? GetVelocity().X / 4 : 0;
		float velY = (doubleJt < 1) ? GetVelocity().Y / 4 : 0;
		ACharacter::LaunchCharacter(FVector(velX, velY, JumpHeight), false, true);
		doubleJt++;
		stamina -= 25.0f;
		if (stamina < 0)
		{
			stamina = 0.0f;
		}
	}
}

void ALudumDare49Character::Landed(const FHitResult& hit)
{
	doubleJt = 0;
}

void ALudumDare49Character::Dodge()
{
	if (Controller != nullptr)
	{

	}
}

void ALudumDare49Character::DepleteStamina(float amount) 
{
	if (Controller != nullptr)
	{

	}
}

void ALudumDare49Character::DepleteHealth(float amount)
{
	if (Controller != nullptr)
	{

	}
}

void ALudumDare49Character::StartSprint()
{
	if (!LockedOn)
	{
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
		sprinting = true;
	}
}

void ALudumDare49Character::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = regularSpeed;
	sprinting = false;
}

