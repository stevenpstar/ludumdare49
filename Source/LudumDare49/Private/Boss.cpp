// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/CapsuleComponent.h"

FName ABoss::CapsuleComponentName(TEXT("CollisionCylinderOfBoss"));
// Sets default values
ABoss::ABoss()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

//	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(ABoss::CapsuleComponentName);
//	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
//	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
//	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
//	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
//	CapsuleComponent->SetCanEverAffectNavigation(false);
//	CapsuleComponent->bDynamicObstacle = true;
//	RootComponent = CapsuleComponent;

//	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
//	Mesh->SetupAttachment(CapsuleComponent);
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

