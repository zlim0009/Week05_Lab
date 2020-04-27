// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameStateMachine.h"
#include "EnemyPawn.generated.h"

UCLASS()
class WEEK2_LAB_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	enum Enemy_States
	{
		STATE_DO_NOTHING = 0,
		CHASE_STATE,
		FLEE_STATE,
		WAIT_STATE,
		ATTACK_STATE,
		CONFUSED_STATE
	};
	
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetSpeed(float NewSpeed);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	Enemy_States GetCurrState() { return m_StateMachine->GetCurrentState(); }
	void SetCurrState(Enemy_States state) { m_StateMachine->ChangeState(state); }

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	float speed = 200.0f;

	StateMachine<Enemy_States, AEnemyPawn>* m_StateMachine;

	void State_Chase_OnEnter(void);
	void State_Chase_OnTick(float f_DeltaTime);
	void State_Chase_OnExit(void);

	void State_Flee_OnEnter(void);
	void State_Flee_OnTick(float f_DeltaTime);
	void State_Flee_OnExit(void);

	void State_Wait_OnEnter(void);
	void State_Wait_OnTick(float f_DeltaTime);
	void State_Wait_OnExit(void);

	void State_Attack_OnEnter(void);
	void State_Attack_OnTick(float f_DeltaTime);
	void State_Attack_OnExit(void);

	void State_Confused_OnEnter(void);
	void State_Confused_OnTick(float f_DeltaTime);
	void State_Confused_OnExit(void);

	float m_timeElapse = 0.0f;
	float m_waitTime = (rand() / (float)RAND_MAX) * 2 + 2;
	float m_BraveryValue = (rand() / (float)RAND_MAX) * 2 + 1;
	int m_ConfuseValue;
};
