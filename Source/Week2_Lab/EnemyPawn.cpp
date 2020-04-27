// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPawn.h"

#include <iterator>

#include "Engine/GameEngine.h"
#include "UObject/ConstructorHelpers.h"

#include "Components/StaticMeshComponent.h"
#include "PlayerPawn.h"


// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));

	// Getting the Enemy shape
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Wedge(TEXT("‪StaticMesh'/Game/StarterContent/Shapes/Shape_Wedge_A.Shape_Wedge_A'"));

	// Assigning shape to Enemy Mesh (Via casting to UStaticMeshComponent)
	Mesh = Cast<UStaticMeshComponent>(RootComponent);
	Mesh->SetStaticMesh(Wedge.Object);
	Mesh->SetSimulatePhysics(true);

	// StateMachine setup and register
	m_StateMachine = new StateMachine<Enemy_States, AEnemyPawn>(this, STATE_DO_NOTHING);

	m_StateMachine->RegisterState(CHASE_STATE, &AEnemyPawn::State_Chase_OnEnter, &AEnemyPawn::State_Chase_OnTick, &AEnemyPawn::State_Chase_OnExit);
	m_StateMachine->RegisterState(FLEE_STATE, &AEnemyPawn::State_Flee_OnEnter, &AEnemyPawn::State_Flee_OnTick, &AEnemyPawn::State_Flee_OnExit);
	m_StateMachine->RegisterState(WAIT_STATE, &AEnemyPawn::State_Wait_OnEnter, &AEnemyPawn::State_Wait_OnTick, &AEnemyPawn::State_Wait_OnExit);
	m_StateMachine->RegisterState(ATTACK_STATE, &AEnemyPawn::State_Attack_OnEnter, &AEnemyPawn::State_Attack_OnTick, &AEnemyPawn::State_Attack_OnExit);
	m_StateMachine->RegisterState(CONFUSED_STATE, &AEnemyPawn::State_Confused_OnEnter, &AEnemyPawn::State_Confused_OnTick, &AEnemyPawn::State_Confused_OnExit);

	m_StateMachine->ChangeState(CHASE_STATE);
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	m_StateMachine->Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Set new speed of Enemy
void AEnemyPawn::SetSpeed(float NewSpeed)
{
	speed = NewSpeed;
}

// Attack States
void AEnemyPawn::State_Attack_OnEnter() {}

void AEnemyPawn::State_Attack_OnTick(float f_DeltaTime)
{
	FVector toPlayer = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
	if (toPlayer.Size() > 200.0f)
	{
		m_StateMachine->ChangeState(CHASE_STATE);
	}
	else
	{
		APlayerPawn* player = Cast<APlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		player->TakeDamage(10.0f);
	}
}

void AEnemyPawn::State_Attack_OnExit() {}

// Chase States
void AEnemyPawn::State_Chase_OnEnter() {}

void AEnemyPawn::State_Chase_OnTick(float f_DeltaTime)
{
	FVector toPlayer = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
	if (toPlayer.Size() < 200.0f)
	{
		m_StateMachine->ChangeState(ATTACK_STATE);
	}
	toPlayer.Normalize();

	FVector NewLocation = GetActorLocation() + (toPlayer * speed * f_DeltaTime);
	SetActorLocation(NewLocation);
}

void AEnemyPawn::State_Chase_OnExit() {}

// Flee States
void AEnemyPawn::State_Flee_OnEnter() { m_timeElapse = 0.0f; }

void AEnemyPawn::State_Flee_OnTick(float f_DeltaTime)
{
	FVector toPlayer = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
	toPlayer.Normalize();
	FVector NewLocation = GetActorLocation() - (toPlayer * speed * f_DeltaTime);
	SetActorLocation(NewLocation);

	m_timeElapse += f_DeltaTime;
	if (m_timeElapse > m_BraveryValue)
	{
		m_timeElapse = 0.0f;
		m_StateMachine->ChangeState(WAIT_STATE);
	}
}

void AEnemyPawn::State_Flee_OnExit() {}

// Wait States
void AEnemyPawn::State_Wait_OnEnter() {}

void AEnemyPawn::State_Wait_OnTick(float f_DeltaTime)
{
	m_timeElapse += f_DeltaTime;
	if (m_timeElapse > m_waitTime)
	{
		m_timeElapse = 0.0f;

		m_ConfuseValue = rand() % 10 + 1;
		
		if (m_ConfuseValue >= 3)
		{
			m_StateMachine->ChangeState(CHASE_STATE);
		}
		else 
		{
			m_StateMachine->ChangeState(CONFUSED_STATE);
		}
		
	}
}

void AEnemyPawn::State_Wait_OnExit() {}

// Confused States
void AEnemyPawn::State_Confused_OnEnter() {}

void AEnemyPawn::State_Confused_OnTick(float f_DeltaTime)
{
	// Get all enemies in world
	// I TRIED TO ITERATE BUT CANNOT PLS FIX
	TArray<AEnemyPawn*> EnemyList;
	for (TActorIterator<AEnemyPawn> EnemyItr(GetWorld()); EnemyItr; ++EnemyItr)
	{
		// Check if EnemyIter is not this enemy
		if (EnemyItr != this)
		{
			EnemyList.Add(Cast<AEnemyPawn>(*EnemyItr));
		}
	}
	
	// Get nearest enemy
	float distanceToThis;
	AEnemyPawn* nearestEnemy;
	for (auto enemy:EnemyList)
	{
		FVector vectorToThis = enemy->GetActorLocation() - GetActorLocation();
		if (vectorToThis.Size() > distanceToThis)
		{
			nearestEnemy = enemy;
		}
	}
	
	// Move to nearest enemy
	FVector toEnemy = nearestEnemy->GetActorLocation() - GetActorLocation();
	toEnemy.Normalize();
	FVector NewLocation = GetActorLocation() - (toEnemy * speed * f_DeltaTime);
	SetActorLocation(NewLocation);

	
	m_timeElapse += f_DeltaTime;
	if (m_timeElapse > m_waitTime)
	{
		m_timeElapse = 0.0f;
		m_StateMachine->ChangeState(CHASE_STATE);
	}
}

void AEnemyPawn::State_Confused_OnExit() {}