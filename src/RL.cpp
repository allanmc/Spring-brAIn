#include "RL.h"

using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{
	vector<float> f;
	
	ai = aiClasses;

	ValueFunction = new RL_Q(	ai,
								RL_PLANT_INDEX*RL_LAB_INDEX*RL_MEX_INDEX*RL_SOLAR_INDEX,
								RL_ACTION_INDEX,
								DataDirs::GetInstance(ai->callback)->GetWriteableDir());

	ParentNode[0] = -1; //no parent
	ParentNode[1] = 0;
	ParentNode[2] = 0;

	Epsilon = 9;
<<<<<<< HEAD
	LoadFromFile();

	totalReward = 0.0;
	goalAchieved = false;

	loopCounter = 0;
=======
	PreviousState = NULL;
	PreviousAction = NULL;
>>>>>>> 996abe9cf5e17714255e2eb1b4a657304a2fddc9
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
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: actionID=%d unitdef=%d", action->ID, action->Action );
	}
	else //greedy
	{
		action = FindBestAction(state);
		ai->utility->Log( ALL, LOG_RL, "Greedy: actionID=%d unitdef=%d", action->ID, action->Action );
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

<<<<<<< HEAD
RL_Action* RL::SafeNextAction(RL_State *state)
{
	RL_State *tmpCurrentState = state;
	int tmpCurrentNode = currentNode;

	while(state->GetActions().size() > 0)
	{
		RL_Action *nextAction = FindNextAction( state );
		if(nextAction->Complex)
		{
			tmpCurrentState = GetState(nextAction->Action);
			if(tmpCurrentState->GetActions().size() == 0)
			{
				state->DeleteAction(nextAction);
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
	return NULL;
}

void RL::TakeAction(RL_Action* action)
{
	//should only be called with complex actions!
	if(!action->Complex)
		return;
	
	currentNode = action->Action;
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:TakeAction() new node:%d", currentNode);
	//clean the "scope"
	PreviousAction[currentNode] = NULL;
	PreviousFrame[currentNode] = 0;
	PreviousState[currentNode] = NULL;
}

RL_Action* RL::Update()
{
	if(loopCounter++ >= 5000)
	{
		return NULL;
	}

	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() node:%d", currentNode);

	bool terminal = false;
	RL_State *state = GetState(currentNode);
	RL_Action *nextAction = SafeNextAction(state);
	
	//Start state
	if ( PreviousState[currentNode] == NULL )
	{
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = ai->frame;
		if(nextAction->Complex)
		{
			TakeAction(nextAction);
			return Update();
		}
		else
			return nextAction;
	}//else

	//Reward
	float reward = -(ai->frame - PreviousFrame[currentNode])/30;
	float bestFutureValue;
	if ( state->IsTerminal() )
=======
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
>>>>>>> 996abe9cf5e17714255e2eb1b4a657304a2fddc9
	{
		reward += 100;
		terminal = true;
		bestFutureValue = 0;//no future actions to take
	}
<<<<<<< HEAD
	else
	{
		RL_Action *bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction[currentNode]->GetValue(state, bestAction);
	}

	//if complex then update the childs value funtion
	if(PreviousAction[currentNode]->Complex)
	{
		int subNode = PreviousAction[currentNode]->Action;
		float subValue = ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) 
						+ ALPHA*(
							reward + GAMMA*bestFutureValue 
							- ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) );

		ValueFunction[subNode]->SetValue(PreviousState[subNode],PreviousAction[subNode], subValue);
	}

	//update own value function
	float value = ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) 
				+ ALPHA*(
					reward + GAMMA*bestFutureValue 
					- ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) );

	ValueFunction[currentNode]->SetValue(PreviousState[currentNode],PreviousAction[currentNode], value);


	delete PreviousState[currentNode];
	delete PreviousAction[currentNode];
	PreviousState[currentNode] = state;
	PreviousAction[currentNode] = nextAction;
	PreviousFrame[currentNode] = ai->frame;

	if ( terminal )
	{
		if(ParentNode[currentNode] == -1)//root check
		{
			goalAchieved = true;
			return NULL;//MEANS THAT YOU SHOULD STOP NOW!!
		}
		else
		{
			currentNode = ParentNode[currentNode];//parentNode
			return Update();//recursive call
		}
	}
	else
	{
		if(nextAction->Complex)
		{
			TakeAction(nextAction);
			return Update();
		}
		else
		{
			return nextAction;
		}
	}
}

=======

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
>>>>>>> 996abe9cf5e17714255e2eb1b4a657304a2fddc9
