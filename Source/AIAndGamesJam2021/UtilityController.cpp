// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilityController.h"

#include <string>

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

bool AUtilityAction::IsActionPossible_Implementation(AUtilityController* Controller)
{
	return true;
}

float AUtilityConsideration::GetConsideration_Implementation(AUtilityController* Controller)
{
	return 0.0f;
}

float AUtilityAction::CalculateConsideration(AUtilityController* Controller)
{
	//If there is no considerations
	if (UtilityConsiderations.Num() == 0)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("No consideration found for %f"), *GetName()));
		return 0.0f;
	}

	AUtilityConsideration* FirstConsideration = NewObject<AUtilityConsideration>(this, UtilityConsiderations[0].Get());
	
	//If there is only one consideration
	if (UtilityConsiderations.Num() == 1)
	{
		const float Result = FirstConsideration->GetConsideration(Controller);
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Consideration for %s returned: %f"), *GetName(), Result));
		return Result;
	}

	//Multiply all considerations together
	float Consideration = FirstConsideration->GetConsideration(Controller);		
	for (int i = 1; i < UtilityConsiderations.Num(); i++)
	{
		AUtilityConsideration* CurrentConsideration = NewObject<AUtilityConsideration>(this, UtilityConsiderations[i].Get());
		
		Consideration *= CurrentConsideration->GetConsideration(Controller);
	}

	//Apply compensation
	const float ModificationFactor = 1 - (1 / UtilityConsiderations.Num());
	const float MakeUpValue = (1 - Consideration) * ModificationFactor;
	const float Result = (Consideration + (MakeUpValue * Consideration)) * ConsiderationWeight;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Considerations for %s returned: %f"), *GetName(), Result));
	
	return Result;
}

bool AUtilityAction::PerformAction_Implementation(AUtilityController* Controller)
{
	return true;
}

AUtilityController::AUtilityController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void AUtilityController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Blackboard->InitializeBlackboard(*BlackboardData);
}

void AUtilityController::PerformBestAction()
{
	struct FAction
	{
	public:
		FAction(){};
		FAction(AUtilityAction* Action_, float Consideration_)
		{
			Action = Action_;
			Consideration = Consideration_;
		};
		
		AUtilityAction* Action = nullptr;
		float Consideration;
	};

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Utility controller %s is looking for best action"), *GetName()));
	FAction BestAction;
	
	//Iterate through all actions
	for (int i = 0; i < Actions.Num(); i++)
	{
		AUtilityAction* CurrentAction = NewObject<AUtilityAction>(this, Actions[i].Get());
		
		//If action is not possible skip
		if (!CurrentAction->IsActionPossible(this))
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Action %s is not possible"), *CurrentAction->GetName()));
			continue;
		}

		//If new action is greater than current or current is null
		if (BestAction.Action == nullptr || CurrentAction->CalculateConsideration(this) > BestAction.Consideration)
			BestAction = FAction(CurrentAction, CurrentAction->CalculateConsideration(this));
	}

	//Perform best action if action exists
	if (BestAction.Action != nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Performing action %s"), *BestAction.Action->GetName()));
		BestAction.Action->PerformAction(this);
	}
}
