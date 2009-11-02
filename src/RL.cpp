#include "RL.h"



using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{	
	ai = aiClasses;
	
	currentNode = 0;

	for(int i = 0; i < RL_NUM_NODES; i++)
	{
		PreviousState[i] = NULL;
		PreviousAction[i] = NULL;
	}

	ValueFunction[0] = new RL_Q(ai,2*2/*states*/,2/*actions*/); //root
	ValueFunction[1] = new RL_Q(ai,RL_LAB_INDEX*RL_PLANT_INDEX/*states*/,2/*actions*/); //Factory
	ValueFunction[2] = new RL_Q(ai,RL_SOLAR_INDEX*RL_MEX_INDEX/*states*/,2/*actions*/); //Resource

	Epsilon = 9;
	LoadFromFile();

	totalReward = 0.0;
	goalAchieved = false;
}

RL::~RL()
{
	SaveToFile();
	for ( int i = 0 ; i < 3 ; i++ )
	{
		delete ValueFunction[i];
		delete PreviousAction[i];
		delete PreviousState[i];
	}
	if (goalAchieved)
	{
		ai->utility->ChatMsg("RL goal achieved with total reward: %f", totalReward);
	} 
	else
	{
		ai->utility->ChatMsg("RL goal NOT achieved, but total reward is: %f", totalReward);
	}
}

void RL::LoadFromFile()
{
	const char* dir = ai->callback->GetDataDirs()->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "qh.bin");

	FILE* fp = NULL;
	fp = fopen( path, "rb" );
	if( fp != NULL )
	{
		fclose( fp );
		//load stuff

		FileHeader fileHeader;
		ifstream *readFile = new ifstream(path, ios::binary | ios::in);
		
		ai->utility->Log(ALL, LOG_RL, "I am going to read RL file!");
		
		readFile->read( (char*)&fileHeader, sizeof(FileHeader) );
		if (fileHeader.header[0]==FILE_HEADER[0] &&
			fileHeader.header[1]==FILE_HEADER[1] &&
			fileHeader.type==1)
		{
			for(int i = 0 ; i < fileHeader.numQTables; i++)
			{
				ValueFunction[i]->LoadFromFile(readFile);
			}
		}
		delete readFile;
	}
	else
	{
		ValueFunction[0]->Clear();
		ValueFunction[1]->Clear();
		ValueFunction[2]->Clear();
	}
	delete[] path;
}

void RL::SaveToFile()
{
	const char* dir = ai->callback->GetDataDirs()->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "qh.bin");

	ofstream *file = new ofstream(path, ios::binary | ios::out);

	FileHeader fileHeader;

	fileHeader.header[0] = FILE_HEADER[0];
	fileHeader.header[1] = FILE_HEADER[1];
	fileHeader.numQTables = 3;
	fileHeader.type = 1;


	file->write( (char*)&fileHeader, sizeof(fileHeader) );

	for(int i = 0; i< fileHeader.numQTables; i++)
	{
		ValueFunction[i]->SaveToFile(file);
	}

	file->flush();
	file->close();
	delete[] path;
	delete file;
}

RL_State* RL::GetState(int node)
{
	RL_State* state;
	switch(node)
	{
	case 0:
		state = new RL_State_Root(ai);
		break;
	case 1:
		state = new RL_State_Factory(ai);
		break;
	case 2:
		state = new RL_State_Resource(ai);
		break;
	default:
		state = NULL;
	}

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

	float bestValue = ValueFunction[currentNode]->GetValue(state, action);

	vector<RL_Action*>::iterator it;
	for ( it = stateActions.begin()+1 ; it != stateActions.end() ; it++ )
	{
		RL_Action *tempAction = (RL_Action*)(*it);
		if ( ValueFunction[currentNode]->GetValue(state, tempAction) > bestValue )
		{
			bestValue = ValueFunction[currentNode]->GetValue(state, tempAction);
			action = tempAction;
		}
	}

	return action;
}

RL_Action* RL::SafeNextAction(RL_State *state)
{
	RL_State *tmpCurrentState = state;
	int tmpCurrentNode = CurrentNode;

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
				/*
				//all good... go down in the tree!
				CurrentNode = nextAction->Action();
				PreviousAction[CurrentNode] = NULL;
				PreviousFrame = 0;
				PreviousState = NULL;
				return TakeAction(tmpCurrentState);*/
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
	
	CurrentNode = nextAction->Action();
	PreviousAction[CurrentNode] = NULL;
	PreviousFrame = 0;
	PreviousState = NULL;
}

RL_Action* RL::Update()
{
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
	if ( state->IsTerminal() )
	{
		if(currentNode == 0)
			reward += 100;
		
		terminal = true;
	}
	totalReward += reward;



}

RL_Action* RL::UpdateBackup()
{
	ai->utility->Log( ALL, LOG_RL, "update node:%d", currentNode );
	/*if(currentNode != 0)
		return new RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),0, false);*/

	bool terminal = false;
	RL_State *state = GetState(currentNode);
	RL_Action *nextAction = FindNextAction( state );

	if ( PreviousState[currentNode] == NULL )
	{
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = ai->frame;
		if(!nextAction->Complex)
			return nextAction;
		else
		{
			ai->utility->Log( ALL, LOG_RL, "complex action(prev null) node:%d actionId:%d", currentNode, nextAction->ID );
			currentNode = (currentNode == 0 ? nextAction->ID + 1 : 0 );
			return Update();//recursive call
		}
	}


	float reward = -(ai->frame - PreviousFrame[currentNode])/30;
	if ( state->IsTerminal() )
	{
		if(currentNode == 0)
			reward += 100;
		
		terminal = true;
	}
	totalReward += reward;

	float bestFutureValue;
	if(terminal)
		bestFutureValue = 0;
	else
	{	
		RL_Action *bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction[currentNode]->GetValue(state, bestAction);
	}
	if(currentNode == 0)
	{//Update the subNode's value function
		int subNode = PreviousAction[currentNode]->ID + 1;

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
		if(currentNode == 0)
		{
			goalAchieved = true;
			return new RL_Action(-1,-1, false);//MEANS THAT YOU SHOULD STOP NOW!!
		}
		else
		{
			currentNode = 0;//parentNode
			return Update();//recursive call
		}
	}

	if(!nextAction->Complex)
		return nextAction;
	else
	{
		ai->utility->Log( ALL, LOG_RL, "complex action node:%d actionId:%d", currentNode, nextAction->ID );
		currentNode = (currentNode == 0 ? nextAction->ID + 1 : 0 );
		delete PreviousState[currentNode];
		PreviousState[currentNode] = NULL;
		delete PreviousAction[currentNode];
		PreviousAction[currentNode] = NULL;
		return Update();//recursive call
	}
}