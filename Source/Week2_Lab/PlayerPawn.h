// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "PlayerPawn.generated.h"

UCLASS()
class WEEK2_LAB_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float speed = 500.0f;
	FVector MovementInput;
	float health = 500.0f;
	float damageCooldown = 0;
	

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Explosion();
	void SlowDown();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* VisibleComponent;
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		UMaterialInterface* HurtMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		UMaterialInterface* UnhurtMaterial;

	void TakeDamage(float Damage);
};
