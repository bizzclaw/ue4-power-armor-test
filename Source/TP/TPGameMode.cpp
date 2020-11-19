// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPGameMode.h"
#include "TPCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPGameMode::ATPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
