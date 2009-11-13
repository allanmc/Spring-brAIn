#include "ReinforcementLearningNode.h"


using namespace std;
using namespace springai;
using namespace brainSpace;


ReinforcementLearningNode::ReinforcementLearningNode(brainSpace::AIClasses *aiClasses, std::vector<QAction> actions, std::vector<QStateVar> states, ReinforcementLearningNode* parent )
{
	ValueFunction = new RL_Q( aiClasses, actions, states );

	ai = aiClasses;
	nullState = RL_State( ai, -1 );
	nullAction = RL_Action( -1, -1, false );
	
	
	PreviousAction = nullAction;
	PreviousState = nullState;
	PreviousFrame = -1;

	Parent = parent;
}

ReinforcementLearningNode::~ReinforcementLearningNode()
{
	delete ValueFunction;
}


RL_Action ReinforcementLearningNode::Update()
{
	RL_State currentState = GetState();
	RL_Action newAction = CalcSafeNextAction( currentState );
	RL_Action actionFromChild;
	if ( PreviousAction.Complex )
	{
		actionFromChild = newAction.Node->Update();
	}
	
	UpdateValueFunction();
	PreviousState = currentState;
	PreviousAction = newAction;

	return actionFromChild;
}

void ReinforcementLearningNode::UpdateValueFunction()
{
	if ( PreviousAction == nullAction )
	{
		return;
	}

	float reward = PreviousAction.Node->GetReward();

	RL_State state = GetState();
	RL_Action bestAction = CalcBestAction( state );
	float totalReward = ValueFunction->GetValue( PreviousState, PreviousAction ) + ALPHA*( reward + GAMMA*( ValueFunction->GetValue(state, bestAction) -  ValueFunction->GetValue( PreviousState, PreviousAction ) ) );
	ValueFunction->SetValue( PreviousState, PreviousAction, totalReward );
}


float ReinforcementLearningNode::GetReward()
{
	if ( PreviousAction == nullAction )
		return 0;
	return -( ai->frame - PreviousFrame )/30.0f;
}

RL_State ReinforcementLearningNode::GetState()
{
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
	int EnoughLabs = (labCount >= 4);
	bool terminal = (EnoughLabs ? true : false);

	int affordable = ai->knowledge->selfInfo->resourceInfo->IsAffordableToBuild(ai->utility->GetUnitDef("armcom"),
		ai->utility->GetUnitDef("armlab"));
	
	bool CanBuildLab = (affordable == 0);

	int id = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
	vector<RL_Action> actions;
	actions.push_back(RL_Action(1,0,true));
	actions.push_back(RL_Action(2,1,true));
	return RL_State( ai, actions, id, terminal );
}


RL_Action ReinforcementLearningNode::CalcNextAction(brainSpace::RL_State &state)
{
	vector<RL_Action> stateActions = state.GetActions();
	RL_Action action;
	
	float r = rand() / (float)RAND_MAX;
	if ( r <= EPSILON ) //non-greedy
	{
		action = stateActions[rand()%stateActions.size()];
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: actionID=%d unitdef=%d", action.ID, action.Action );
	}
	else //greedy
	{
		action = CalcBestAction(state);
		ai->utility->Log( ALL, LOG_RL, "Greedy: actionID=%d unitdef=%d", action.ID, action.Action );
	}
	return action;
}

RL_Action ReinforcementLearningNode::CalcBestAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();

	RL_Action action;
	

	float bestValue = ValueFunction->GetValue(state, action);

	for ( int i = 1 ; i < (int)stateActions.size() ; i++ )
	{
		RL_Action tempAction = stateActions[i];
		float tempValue = ValueFunction->GetValue(state, tempAction);
		
		if ( tempValue > bestValue )
		{
			bestValue = tempValue;
			action = tempAction;
		}
	}
	if (action.ID < 0 || action.ID > 2)
		exit(0);
	return action;
}


RL_Action ReinforcementLearningNode::CalcSafeNextAction(RL_State &state)
{
	RL_State tmpCurrentState = state;
	//int tmpCurrentNode = currentNode;

	while(state.GetActions().size() > 0)
	{
		RL_Action nextAction = CalcNextAction( state );
		
		if(nextAction.Complex)
		{
			tmpCurrentState = nextAction.Node->GetState();
			//tmpCurrentState = GetState(nextAction.Action);
			if(tmpCurrentState.GetActions().size() == 0)
			{
				state.DeleteAction(nextAction.ID);
			}
			else
			{
				return nextAction;
			}
		}
		else
		{
			return nextAction;
		}
	}

	return nullAction;
}
