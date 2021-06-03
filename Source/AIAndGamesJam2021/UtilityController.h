// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "UtilityController.generated.h"

//Forward declaration
class AUtilityController;

UCLASS(Blueprintable, BlueprintType)
class AUtilityConsideration : public AActor
{
	GENERATED_BODY()
public:
	AUtilityConsideration(){};
	UFUNCTION(BlueprintNativeEvent)
	float GetConsideration(AUtilityController* Controller);
};

UCLASS(Blueprintable, BlueprintType)
class AUtilityAction : public AActor
{
	GENERATED_BODY()
public:
	AUtilityAction(){};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AUtilityConsideration>> UtilityConsiderations;

	UFUNCTION(BlueprintNativeEvent)
	bool IsActionPossible(AUtilityController* Controller);
	
	virtual float CalculateConsideration(AUtilityController* Controller);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ConsiderationWeight = 1.0f;

	UFUNCTION(BlueprintNativeEvent)
	bool PerformAction(AUtilityController* Controller);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ActionName;
};

UCLASS(Blueprintable, BlueprintType)
class AIANDGAMESJAM2021_API AUtilityController : public AActor
{
	GENERATED_BODY()

	AUtilityController(){};
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AUtilityAction>> Actions;

	UFUNCTION(BlueprintCallable, CallInEditor)
	AUtilityAction* PerformBestAction();
};
