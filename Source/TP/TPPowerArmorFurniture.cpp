// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPowerArmorFurniture.h"
#include "TPCharacter.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ATPPowerArmorFurniture::ATPPowerArmorFurniture()
{
	SetReplicates(true);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));

	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SphereComp->SetupAttachment(MeshComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetRelativeLocation(FVector(0, 0, 0));
	CameraComp->SetupAttachment(MeshComp);

	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ATPPowerArmorFurniture::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ATPPowerArmorFurniture::ServerPowerArmorEnter_Validate(ATPCharacter* Character, APlayerController* PC)
{
	if (Character->bInPowerArmor || Occupant != nullptr)
	{
		return false;
	}

	float Distance = this->GetDistanceTo(Character);

	if (Distance <= (SphereComp->GetScaledSphereRadius() + 128.0f))
	{
		return true;
	}
	else {
		return false;
	}
}
void ATPPowerArmorFurniture::ServerPowerArmorEnter_Implementation(ATPCharacter* Character, APlayerController* PC)
{

	Occupant = Character;
	SetOwner(Character);

	PC->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);

	GetWorldTimerManager().ClearTimer(TimerHandle_EnterPowerArmor);

	FTimerDelegate EnterDel;
	EnterDel.BindUFunction(this, FName(TEXT("FinishEnter")), Character);
	GetWorldTimerManager().SetTimer(TimerHandle_EnterPowerArmor, EnterDel, PowerArmorEnterAnimation->GetPlayLength(), false);


	Character->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Character->AddActorLocalOffset(FVector(0, 0, 73.07)); // TODO: Figure out how to get the exact offset of the mesh component with the scale applied and use that instead

	NetMulticastPowerArmorAnims(Character, PowerArmorEnterAnimation, Character->PowerArmorEnterAnimation);
}

void ATPPowerArmorFurniture::NetMulticastPowerArmorAnims_Implementation(ATPCharacter* Character, UAnimationAsset* FurnitureAnim, UAnimationAsset* CharacterAnim)
{	
	Character->DisableInput(nullptr);
	Character->bInPowerArmor = true;

	UPawnMovementComponent* CharacrterMovementComp = Character->GetMovementComponent();
	CharacrterMovementComp->Velocity = FVector(0, 0, 0);

	TArray<USkeletalMeshComponent*> Components;
	Character->GetComponents<USkeletalMeshComponent>(Components);
	USkeletalMeshComponent* CharacterMeshComp = Components[0];

	if (CharacterMeshComp && Character->PowerArmorEnterAnimation && FurnitureAnim) {
		MeshComp->PlayAnimation(FurnitureAnim, false);
		CharacterMeshComp->PlayAnimation(CharacterAnim, false);
	}
}

void ATPPowerArmorFurniture::FinishEnter(ATPCharacter* Character)
{	
	if (!Character)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());

	if (!PC)
	{
		return;
	}
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ATPCharacter* PowerArmorCharacter = GetWorld()->SpawnActor<ATPCharacter>(PowerArmorClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
	PowerArmorCharacter->bInPowerArmor = true;
	PC->Possess(PowerArmorCharacter);
	PowerArmorCharacter->EnableInput(PC);

	Character->Destroy();
	Destroy();
}


void ATPPowerArmorFurniture::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	ATPCharacter* Character = Cast<ATPCharacter>(OtherActor);
	APlayerController* PC = Cast<APlayerController>(Character->GetController());

	if (PowerArmorClass && Character && PC && !Character->bInPowerArmor)
	{
		ServerPowerArmorEnter(Character, PC);
	}
}