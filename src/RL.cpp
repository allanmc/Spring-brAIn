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

	Actions.push_back( new RL_Action( -1, 3 ) );


	ValueFunction = new RL_Q(RL_LAB_INDEX*RL_MEX_INDEX*RL_SOLAR_INDEX,RL_ACTION_INDEX,DataDirs::GetInstance(ai->callback)->GetWriteableDir());

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
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armmex" );
	int mexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armlab" );
	RL_State *state = new RL_State(ai,labCount,solarCount,mexCount);
	ai->utility->Log( ALL, LOG_RL, "Solar: %d. Lab: %d. Mex: %d. State: %d", solarCount, labCount, mexCount, state );
	return state;
}

RL_Action *RL::FindNextAction( RL_State* state )
{
	RL_Action* action = Actions[0]; //unitdefID
	
	int r = rand()%100;
	if ( r <= Epsilon ) //non-greedy
	{
		action = Actions[rand()%RL_ACTION_INDEX];
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: id=%d unitdef=", action->ID, action->UnitDefID );
	}
	else //greedy
	{
		action = FindBestAction(state);
		ai->utility->Log( ALL, LOG_RL, "Greedy: id=%d unitdef=", action->ID, action->UnitDefID );
	}
	return action;
}

RL_Action *RL::FindBestAction( RL_State* state )
{
	RL_Action *action = Actions[0]; //unitdefID

	float bestValue = ValueFunction->GetValue(state, action);

	vector<RL_Action*>::iterator it;
	for ( it = Actions.begin()+1 ; it != Actions.end() ; it++ )
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
		return nextAction;

	int reward = 0;
	if ( ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armlab" ) == 4 )
	{
		reward = 100;
		terminal = true;
	}

	float value = ValueFunction->GetValue(PreviousState,PreviousAction) 
		+ ALPHA*(
			reward + GAMMA*ValueFunction->GetValue(state, bestAction) 
			- ValueFunction->GetValue(PreviousState,PreviousAction) );

	ValueFunction->SetValue(PreviousState,PreviousAction, value);
	PreviousState = state;
	PreviousAction = nextAction;

	if ( terminal )
	{
		ValueFunction->SaveToFile();
		return new RL_Action(-1,-1);//MEANS THAT YOU SHOULD STOP NOW!!
	}
	return nextAction;
	
}