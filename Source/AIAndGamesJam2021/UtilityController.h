// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UtilityController.generated.h"

class UtilityConsideration
{
public:
	virtual float GetConsideration() { return 0; };
};

//Forward declaration
class AUtilityController;

USTRUCT()
struct FUtilityAction
{
	virtual ~FUtilityAction() = default;
	GENERATED_BODY()

	TArray<UtilityConsideration*> UtilityConsiderations;

	virtual bool IsActionPossible() { return false; }
	
	virtual float CalculateConsideration();

	float ConsiderationWeight = 1.0f;
	
	virtual bool PerformAction(AUtilityController* Controller) { return false; }
};

UCLASS(Blueprintable)
class AIANDGAMESJAM2021_API AUtilityController : public AAIController
{
	GENERATED_BODY()

	TArray<FUtilityAction*> Actions;

	UFUNCTION(BlueprintCallable)
	void PerformBestAction();
};
