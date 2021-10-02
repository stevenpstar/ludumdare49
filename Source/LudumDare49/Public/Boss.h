// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Boss.generated.h"

UCLASS()
class LUDUMDARE49_API ABoss : public APawn
{
	GENERATED_BODY()

	// Capsule Component
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

public:
	// Sets default values for this pawn's properties
	ABoss();

	static FName CapsuleComponentName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Skeletal Mesh Component of the boss
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh;
};
