
#include "RL.h"



using namespace brainSpace;
using namespace std;

RL::RL(Game *g)
{	
	game = g;
	currentNode = 0;
	totalReward = 0;
	nullState = RL_State(game, -1 );
	nullAction = RL_Action( -1, -1, false );

	vector<QStateVar> stateVars;
	vector<QAction> actions;
	stateVars.push_back( QStateVar("Lab", RL_LAB_INDEX));
	stateVars.push_back( QStateVar("Solar", RL_SOLAR_INDEX));
	stateVars.push_back( QStateVar("Mex", RL_MEX_INDEX));
	actions.push_back( QAction("Lab", 0));
	actions.push_back( QAction("Solar", 1));
	actions.push_back( QAction("Mex", 2));
	ValueFunction[0] = new RL_Q( actions, stateVars); //root

	ClearAllNodes();

	ParentNode[0] = -1; //no parent

	//Epsilon = 9;
	LoadFromFile();

	totalReward = 0.0;
	goalAchieved = false;
}

RL::~RL()
{
	SaveToFile();
	for ( int i = 0 ; i < RL_NUM_NODES ; i++ )
	{
		delete ValueFunction[i];
		ValueFunction[i] = NULL;
		//delete PreviousAction[i];
		//PreviousAction[i] = NULL;
		//delete PreviousState[i];
		//PreviousState[i] = NULL;
	}
}

void RL::ClearAllNodes()
{
	for(int i = 0; i < RL_NUM_NODES; i++)
	{
		PreviousState[i] = nullState;
		PreviousAction[i] = nullAction;
		ValueFunction[i]->Clear();
	}
}

void RL::LoadFromFile()
{
	const char* dir = "";

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "qn.bin");

	FILE* fp = NULL;
	fp = fopen( path, "rb" );
	if( fp != NULL )
	{
		fclose( fp );
		//load stuff

		FileHeader fileHeader;
		ifstream *readFile = new ifstream(path, ios::binary | ios::in);
		
		
		
		readFile->read( (char*)&fileHeader, sizeof(FileHeader) );
		if (fileHeader.header[0]==FILE_HEADER[0] &&
			fileHeader.header[1]==FILE_HEADER[1] &&
			fileHeader.type==QBFILE_VERSION)
		{
			for(int i = 0 ; i < fileHeader.numQTables; i++)
			{
				ValueFunction[i]->LoadFromFile(readFile);
			}
		}
		else
		{
			
		}
		delete readFile;
	}
	delete[] path;
}

void RL::SaveToFile()
{
	const char* dir = "";
	
	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "qn.bin");

	ofstream *file = new ofstream(path, ios::binary | ios::out);

	FileHeader fileHeader;

	fileHeader.header[0] = FILE_HEADER[0];
	fileHeader.header[1] = FILE_HEADER[1];
	fileHeader.numQTables = RL_NUM_NODES;
	fileHeader.type = QBFILE_VERSION;


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

RL_State RL::GetState(int node)
{
	return RL_State(game, node);
}

RL_Action RL::FindNextAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();
	RL_Action action = stateActions[0]; //unitdefID
	
	float r = rand()/(float)RAND_MAX;
	if ( r <= EPSILON ) //non-greedy
	{
		action = stateActions[rand()%stateActions.size()];
		
	}
	else //greedy
	{
		action = FindBestAction(state);
		
	}
	return action;
}

RL_Action RL::FindBestAction( RL_State &state )
{
	
	vector<RL_Action> stateActions = state.GetActions();
	

	RL_Action action = stateActions[0]; //unitdefID
	
	float bestValue = ValueFunction[currentNode]->GetValue(state, action);

	

	//vector<RL_Action*>::iterator it;
	//for ( it = stateActions.begin()+1 ; it != stateActions.end() ; it++ )
	for ( int i = 1 ; i < (int)stateActions.size() ; i++ )
	{
		//RL_Action *tempAction = (RL_Action*)(*it);
		RL_Action tempAction = stateActions[i];
		float tempValue = ValueFunction[currentNode]->GetValue(state, tempAction);
		
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

RL_Action RL::SafeNextAction(RL_State &state)
{
	RL_State tmpCurrentState = state;
	//int tmpCurrentNode = currentNode;

	while(state.GetActions().size() > 0)
	{
		RL_Action nextAction = FindNextAction( state );
		if(nextAction.Complex)
		{
			tmpCurrentState = GetState(nextAction.Action);
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

void RL::TakeAction(RL_Action &action)
{
	//should only be called with complex actions!
	if(!action.Complex)
		return;
	
	currentNode = action.Action;
	//clean the "scope"
	PreviousAction[currentNode] = nullAction;
	PreviousFrame[currentNode] = 0;
	PreviousState[currentNode] = nullState;
}

RL_Action RL::Update()
{
	
	bool terminal = false;
	RL_State state = GetState(currentNode);
	RL_Action nextAction = SafeNextAction(state);
	
	
	//Start state
	if ( PreviousState[currentNode] == nullState )
	{
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = game->frame;
		if(nextAction.Complex)
		{
			TakeAction(nextAction);
			return Update();
		}
		else
			return nextAction;
	}//else

	
	//Reward
	float reward = PreviousFrame[currentNode] - game->frame;
	//cout << "previousframe: " << PreviousFrame[currentNode] << "\n";
	//cout << "real-frame: " << game->frame << "\n";
	//cout << "Reward: " << reward << "\n";
	float bestFutureValue;
	if ( state.IsTerminal() )
	{
		if(currentNode == 0)
			reward += 1000;
		
		terminal = true;
		bestFutureValue = 0;//no future actions to take
	}
	else
	{
		RL_Action bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction[currentNode]->GetValue(state, bestAction);
	}
	if(currentNode == 0)
	{
		totalReward += reward;
	}


	//if complex then update the childs value funtion
	if(PreviousAction[currentNode].Complex)
	{
		int subNode = PreviousAction[currentNode].Action;
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
	
	if ( terminal )
	{
		if(ParentNode[currentNode] == -1)//root check
		{
			goalAchieved = true;
			return nullAction;//MEANS THAT YOU SHOULD STOP NOW!!
		}
		else
		{
			currentNode = ParentNode[currentNode];//parentNode
			return Update();//recursive call
		}
	}
	else
	{
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = game->frame;
		
		if(nextAction.Complex)
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

float RL::GetTotalReward()
{
	return totalReward;
}

