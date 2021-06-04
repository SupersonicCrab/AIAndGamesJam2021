// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilityController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("No consideration found for %s"), *GetName()), false);
		return 0.0f;
	}

	AUtilityConsideration* FirstConsideration = NewObject<AUtilityConsideration>(this, UtilityConsiderations[0].Get());
	
	//If there is only one consideration
	if (UtilityConsiderations.Num() == 1)
	{
		const float Result = UKismetMathLibrary::FClamp(FirstConsideration->GetConsideration(Controller), 0.0f, 1.0f) * ConsiderationWeight;
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Consideration for %s returned: %f"), *GetName(), Result), false);
		return Result;
	}

	//Multiply all considerations together
	float Consideration = UKismetMathLibrary::FClamp(FirstConsideration->GetConsideration(Controller), 0.0f, 1.0f);		
	for (int i = 1; i < UtilityConsiderations.Num(); i++)
	{
		AUtilityConsideration* CurrentConsideration = NewObject<AUtilityConsideration>(this, UtilityConsiderations[i].Get());
		
		Consideration *= UKismetMathLibrary::FClamp(CurrentConsideration->GetConsideration(Controller), 0.0f, 1.0f);
	}

	//Apply compensation
	const float ModificationFactor = 1 - (1 / UtilityConsiderations.Num());
	const float MakeUpValue = (1 - Consideration) * ModificationFactor;
	const float Result = (Consideration + (MakeUpValue * Consideration)) * ConsiderationWeight;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Considerations for %s returned: %f"), *GetName(), Result), false);
	
	return Result;
}

bool AUtilityAction::PerformAction_Implementation(AUtilityController* Controller)
{
	return true;
}

AUtilityAction* AUtilityController::PerformBestAction()
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

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Utility controller %s is looking for best action"), *GetName()), false);
	FAction BestAction;
	
	//Iterate through all actions
	for (int i = 0; i < Actions.Num(); i++)
	{
		AUtilityAction* CurrentAction = NewObject<AUtilityAction>(this, Actions[i].Get());
		
		//If action is not possible skip
		if (!CurrentAction->IsActionPossible(this))
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Action %s is not possible"), *CurrentAction->GetName()), false);
			continue;
		}

		float NewScore = CurrentAction->CalculateConsideration(this);
		
		//If new action is greater than current or current is null
		if (BestAction.Action == nullptr || NewScore > BestAction.Consideration)
			BestAction = FAction(CurrentAction, NewScore);
	}

	//Perform best action if action exists
	if (BestAction.Action != nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Performing action %s"), *BestAction.Action->GetName()), false);
		BestAction.Action->PerformAction(this);
		return BestAction.Action;
	}

	//Return nullptr if no action can be taken
	return nullptr;
}
