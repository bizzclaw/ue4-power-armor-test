// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPPowerArmorFurniture.generated.h"

class USkeletalMeshComponent;
class UPrimitiveComponent;
class USphereComponent;
class UCameraComponent;
class ATPCharacter;

UCLASS()
class TP_API ATPPowerArmorFurniture : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPPowerArmorFurniture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ATPCharacter* Occupant;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<AActor> PowerArmorClass;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPowerArmorEnter(ATPCharacter* Character, APlayerController* PC);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastPowerArmorAnims(ATPCharacter* Character, UAnimationAsset* FurnitureAnim, UAnimationAsset* CharacterAnim);

	UPROPERTY(EditDefaultsOnly, Category = "Anims")
	UAnimSequence* PowerArmorEnterAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Anims")
	UAnimSequence* PowerArmorExitAnimation;

	FTimerHandle TimerHandle_EnterPowerArmor;

	UFUNCTION()
	void FinishEnter(ATPCharacter* Character);

public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
};
