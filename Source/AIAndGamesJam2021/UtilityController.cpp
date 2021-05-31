// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilityController.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UUtilityAction::IsActionPossible_Implementation()
{
	return true;
}

float UUtilityConsideration::GetConsideration_Implementation(AUtilityController* Controller)
{
	return 0.0f;
}

float UUtilityAction::CalculateConsideration(AUtilityController* Controller)
{
	//If there is no considerations
	if (UtilityConsiderations.Num() == 0)
		return 0.0f;

	//If there is one consideration
	if (UtilityConsiderations.Num() == 1)
		return UtilityConsiderations[0]->GetConsideration(Controller);

	//Multiply all considerations together
	float Consideration = UtilityConsiderations[0]->GetConsideration(Controller);		
	for (int i = 1; i <= UtilityConsiderations.Num(); i++)
	{
		Consideration *= UtilityConsiderations[i]->GetConsideration(Controller);
	}

	//Apply compensation
	const float ModificationFactor = 1 - (1 / UtilityConsiderations.Num());
	const float MakeUpValue = (1 - Consideration) * ModificationFactor;
	return (Consideration + (MakeUpValue * Consideration)) * ConsiderationWeight;
}

bool UUtilityAction::PerformAction_Implementation(AUtilityController* Controller)
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
		FAction(UUtilityAction* Action_, float Consideration_)
		{
			Action = Action_;
			Consideration = Consideration_;
		};
		
		UUtilityAction* Action = nullptr;
		float Consideration;
	};

	FAction BestAction;

	//Iterate through all actions
	for (int i = 0; i <= Actions.Num(); i++)
	{
		//If action is not possible skip
		if (!Actions[i]->IsActionPossible())
			continue;

		//If new action is greater than current or current is null
		if (BestAction.Action == nullptr || Actions[i]->CalculateConsideration(this) > BestAction.Consideration)
			BestAction = FAction(Actions[i], Actions[i]->CalculateConsideration(this));
	}

	//Perform best action if action exists
	if (BestAction.Action != nullptr)
		BestAction.Action->PerformAction(this);
}
