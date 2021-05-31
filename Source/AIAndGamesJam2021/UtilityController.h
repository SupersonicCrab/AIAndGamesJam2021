// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "UtilityController.generated.h"

//Forward declaration
class AUtilityController;

UCLASS(Blueprintable)
class UUtilityConsideration : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	float GetConsideration(AUtilityController* Controller);
};

UCLASS(Blueprintable)
class UUtilityAction : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UUtilityConsideration*> UtilityConsiderations;

	UFUNCTION(BlueprintNativeEvent)
	bool IsActionPossible();
	
	virtual float CalculateConsideration(AUtilityController* Controller);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ConsiderationWeight = 1.0f;

	UFUNCTION(BlueprintNativeEvent)
	bool PerformAction(AUtilityController* Controller);
};

UCLASS(Blueprintable)
class AIANDGAMESJAM2021_API AUtilityController : public AActor
{
	GENERATED_BODY()

	AUtilityController();

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(BlueprintReadWrite)
	UBlackboardComponent* Blackboard;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBlackboardData* BlackboardData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UUtilityAction*> Actions;

	UFUNCTION(BlueprintCallable)
	void PerformBestAction();
};
