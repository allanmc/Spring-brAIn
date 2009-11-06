#include "RL.h"

using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{
	vector<float> f;
	
	ai = aiClasses;
	goalReached = false;
	totalReward = 0;

	vector<QStateVar> stateVars (4);
	vector<QAction> actions (4);
	
	stateVars[0] = (QStateVar){"Plants", RL_PLANT_INDEX};
	stateVars[1] = (QStateVar){"Labs", RL_LAB_INDEX};
	stateVars[2] = (QStateVar){"Mex", RL_MEX_INDEX};
	stateVars[3] = (QStateVar){"Solar", RL_SOLAR_INDEX};
	actions[0] = (QAction){"Solar", 0};
	actions[1] = (QAction){"Mex", 1};
	actions[2] = (QAction){"Labs", 2};
	actions[3] = (QAction){"Plants", 3};
	
	ValueFunction = new RL_Q(ai, actions, stateVars);
	
	Epsilon = 9;
	PreviousState = NULL;
	PreviousAction = NULL;
}

RL::~RL()
{
	if (goalReached)
	{
		ai->utility->ChatMsg("RL-done - reached goal with reward %i", totalReward);
	} 
	else
	{
		ai->utility->ChatMsg("RL-done - did NOT reach goal with reward %i", totalReward);
	}
}

RL_State* RL::GetState()
{
	int solarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armsolar" );
	int mexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armmex" );
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armlab" );
	int plantCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armvp" );

	RL_State *state = new RL_State( ai, plantCount, labCount,solarCount,mexCount);
	ai->utility->Log( ALL, LOG_RL, "Solar: %d. Lab: %d. Mex: %d. Plant: %d. State: %d", solarCount, labCount, mexCount, plantCount, state->GetID() );
	return state;
}

RL_Action *RL::FindNextAction( RL_State* state )
{
	vector<RL_Action*> stateActions = state->GetActions();
	RL_Action* action = stateActions[0]; //unitdefID
	
	int r = rand()%100;
	if ( r <= Epsilon ) //non-greedy
	{
		action = stateActions[rand()%stateActions.size()];
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: actionID=%d unitdef=%d", action->ID, action->UnitDefID );
	}
	else //greedy
	{
		action = FindBestAction(state);
		ai->utility->Log( ALL, LOG_RL, "Greedy: actionID=%d unitdef=%d", action->ID, action->UnitDefID );
	}
	return action;
}

RL_Action *RL::FindBestAction( RL_State* state )
{
	vector<RL_Action*> stateActions = state->GetActions();

	RL_Action *action = stateActions[0]; //unitdefID

	float bestValue = ValueFunction->GetValue(state, action);

	vector<RL_Action*>::iterator it;
	for ( it = stateActions.begin()+1 ; it != stateActions.end() ; it++ )
	{
		RL_Action *tempAction = (RL_Action*)(*it);
		if ( ValueFunction->GetValue(state, tempAction) > bestValue )
		{
			bestValue = ValueFunction->GetValue(state, tempAction);
			action = tempAction;
		}
	}

	return action;
}

RL_Action *RL::Update( )
{
	bool terminal = false;
	RL_State *state = GetState();
	RL_Action *nextAction = NULL;
	RL_Action *bestAction = NULL;
	float bestValue;

	int reward = -(ai->frame - PreviousFrame)/30;
	if ( state->LabCount == 4 )
	{
		reward += 100;
		terminal = true;
		bestValue = 0; //no further actions to take
	}
	else
	{
		nextAction = FindNextAction( state );
		bestAction = FindBestAction( state );
		bestValue = ValueFunction->GetValue(state, bestAction);
	}
	totalReward += reward;

	if ( PreviousState == NULL )
	{
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = ai->frame;
		return nextAction;
	}

	float value = ValueFunction->GetValue(PreviousState,PreviousAction) 
		+ ALPHA*(
			reward + GAMMA * bestValue
			- ValueFunction->GetValue(PreviousState,PreviousAction) );

	ValueFunction->SetValue(PreviousState,PreviousAction, value);
	PreviousState = state;
	PreviousAction = nextAction;
	PreviousFrame = ai->frame;

	ValueFunction->SaveToFile();//move to terminal later
	if ( terminal )
	{
		goalReached = true;
		return new RL_Action(-1,-1);//MEANS THAT YOU SHOULD STOP NOW!!
	}
	return nextAction;
	
}