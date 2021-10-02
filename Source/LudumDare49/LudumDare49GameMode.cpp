// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudumDare49GameMode.h"
#include "LudumDare49Character.h"
#include "UObject/ConstructorHelpers.h"

ALudumDare49GameMode::ALudumDare49GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/MyLudumDare49Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
