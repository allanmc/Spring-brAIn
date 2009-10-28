#include "RL.h"

using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{
	vector<float> f;
	
	ai = aiClasses;


	Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armsolar" )->GetUnitDefId(), 0 ) );

	Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armmex" )->GetUnitDefId(), 1 ) );

	Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armlab" )->GetUnitDefId(), 2 ) );

	Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armvp" )->GetUnitDefId(), 3 ) );
	

	ValueFunction = new RL_Q(RL_PLANT_INDEX*RL_LAB_INDEX*RL_MEX_INDEX*RL_SOLAR_INDEX,RL_ACTION_INDEX, DataDirs::GetInstance(ai->callback)->GetWriteableDir());

	Epsilon = 9;
	PreviousState = NULL;
	PreviousAction = NULL;
}

RL::~RL()
{
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
	RL_Action *nextAction = FindNextAction( state );
	RL_Action *bestAction = FindBestAction( state );

	if ( PreviousState == NULL )
	{
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = ai->frame;
		return nextAction;
	}

	int reward = -(ai->frame - PreviousFrame)/30;
	if ( state->LabCount == 4 )
	{
		reward += 100;
		terminal = true;
	}

	float value = ValueFunction->GetValue(PreviousState,PreviousAction) 
		+ ALPHA*(
			reward + GAMMA*ValueFunction->GetValue(state, bestAction) 
			- ValueFunction->GetValue(PreviousState,PreviousAction) );

	ValueFunction->SetValue(PreviousState,PreviousAction, value);
	PreviousState = state;
	PreviousAction = nextAction;
	PreviousFrame = ai->frame;

	ValueFunction->SaveToFile();//move to terminal later
	if ( terminal )
	{
		return new RL_Action(-1,-1);//MEANS THAT YOU SHOULD STOP NOW!!
	}
	return nextAction;
	
}