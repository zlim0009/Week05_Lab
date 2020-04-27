// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Engine/GameEngine.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyPawn.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Connecting Components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
	VisibleComponent->SetupAttachment(RootComponent);

	// Camera Component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(-1000.0f, 0.0f, 1000.0f));
	Camera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (damageCooldown > 0)
	{
		damageCooldown -= DeltaTime;
		if (damageCooldown <= 0)
		{
			VisibleComponent->SetMaterial(0, UnhurtMaterial);
		}
	}

	if (!MovementInput.IsZero())
	{
		MovementInput.Normalize();
		FVector NewLoc = GetActorLocation() + (MovementInput * speed * DeltaTime);
		SetActorLocation(NewLoc);
	}
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayerPawn::MoveRight);
	PlayerInputComponent->BindAction("Space", IE_Pressed, this, &APlayerPawn::Explosion);
}

void APlayerPawn::MoveForward(float AxisValue)
{
	MovementInput.X = AxisValue;
}

void APlayerPawn::MoveRight(float AxisValue)
{
	MovementInput.Y = AxisValue;
}

void APlayerPawn::Explosion()
{
	TArray<FHitResult> OutHits;
	FVector Location = GetActorLocation();
	FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(500.0f);

	DrawDebugSphere(GetWorld(), Location, ExplosionSphere.GetSphereRadius(), 50, FColor::Red, false, 2);

	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Location, Location, FQuat::Identity, ECC_WorldStatic, ExplosionSphere);
	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>((Hit.GetActor()->GetRootComponent()));
			if (Mesh)
			{
				Mesh->AddRadialImpulse(Location, 500.0f, 2000.0f, ERadialImpulseFalloff::RIF_Linear, true);
			}

			AEnemyPawn* enemy = Cast<AEnemyPawn>((Hit.GetActor()));
			if (enemy)
			{
				enemy->SetCurrState(AEnemyPawn::FLEE_STATE);
			}
		}
	}
}

void APlayerPawn::TakeDamage(float Damage)
{
	if (damageCooldown <= 0)
	{
		damageCooldown = 1.0f;
		health -= Damage;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Health %f"), health));
		}
		VisibleComponent->SetMaterial(0, HurtMaterial);
	}
}