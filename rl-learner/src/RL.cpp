
#include "RL.h"

using namespace brainSpace;
using namespace std;

RL::RL(Game *g, double epsilon, int numAgents)
{	
	EPSILON = epsilon;
	game = g;
	nullState = RL_State();
	nullAction = RL_Action( -1, -1 );
	vector<QStateVar> stateVars;
	vector<QAction> actions;
	totalReward = 0.0;
	LongestTrace = 0;
	
	for(int i = 0; i<numAgents; i++)
	{
		PreviousFrame.push_back(0);
		PreviousState.push_back(nullState);
		PreviousAction.push_back(nullAction);
	}

	switch(RL_TYPE)
	{
	case 0:

		//stateVars.push_back( QStateVar("Rocko", RL_ROCKO_INDEX));
		stateVars.push_back( QStateVar("Lab", RL_LAB_INDEX));
		stateVars.push_back( QStateVar("Solar", RL_SOLAR_INDEX));
		stateVars.push_back( QStateVar("Mex", RL_MEX_INDEX));		
		actions.push_back( QAction("Lab", 0));
		actions.push_back( QAction("Solar", 1));
		actions.push_back( QAction("Mex", 2));

		
		//actions.push_back( QAction("Rocko", 3));
		ValueFunction = new RL_Q( actions, stateVars); //root
		break;
	case 1://two builders
		stateVars.push_back( QStateVar("ConCur", 4));
		stateVars.push_back( QStateVar("MStore", 4));
		stateVars.push_back( QStateVar("EStore", 2));
		stateVars.push_back( QStateVar("MIncome", 4));
		stateVars.push_back( QStateVar("EIncome", 3));
		stateVars.push_back( QStateVar("LabCount", RL_LAB_INDEX));
		actions.push_back( QAction("Lab", 0));
		actions.push_back( QAction("Solar", 1));
		actions.push_back( QAction("Mex", 2));

		ValueFunction = new RL_Q( actions, stateVars); //root
		break;
	default:
		break;
	}

	dataTrail.clear();
	LoadFromFile();
	goalAchieved = false;
}

RL::~RL()
{
	SaveToFile();
	delete ValueFunction;
	dataTrail.clear();
}

void RL::LoadFromFile()
{
	char *path = GetFilePath();

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
			ValueFunction->LoadFromFile(readFile);
		}
		delete readFile;
	}
	delete[] path;
}

char* RL::GetFilePath()
{
	const char* dir = RL_FILE_PATH;

	char *path = new char[200];
	strcpy(path, dir);

	switch(RL_TYPE)
	{
	case 0:
		strcat(path, RL_FILE_1);
		break;
	case 1:
		strcat(path, RL_FILE_2_BUILDERS);
		break;
	default:
		break;
	}

	return path;

}

void RL::SaveToFile()
{
	char *path = GetFilePath();

	ofstream *file = new ofstream(path, ios::binary | ios::out);

	FileHeader fileHeader;

	fileHeader.header[0] = FILE_HEADER[0];
	fileHeader.header[1] = FILE_HEADER[1];
	fileHeader.type = QBFILE_VERSION;

	file->write( (char*)&fileHeader, sizeof(fileHeader) );

	ValueFunction->SaveToFile(file);

	file->flush();
	file->close();
	delete[] path;
	delete file;
}

RL_State RL::GetState(int agentId)
{
	return RL_State(game, agentId);
}

RL_Action RL::FindNextAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();
	RL_Action action = stateActions[0]; //unitdefID

	float r = rand()/(float)RAND_MAX;
	if ( r <= EPSILON ) //non-greedy
	{
		m_greedyChoice = false;
		action = stateActions[rand()%stateActions.size()];
	}
	else //greedy
	{
		m_greedyChoice = true;
		action = FindBestAction(state);
	}
	return action;
}

RL_Action RL::FindBestAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();
	RL_Action action = stateActions[0]; //unitdefID
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

	return action;
}

RL_Action RL::SafeNextAction(RL_State &state)
{
	if(state.GetActions().size() > 0)
	{
		return FindNextAction( state );
	}	
	return nullAction;
}

RL_Action RL::Update(int agentId)
{
	bool terminal = false;
	RL_State state = GetState(agentId);
	if ( state.GetID() == 1200 )
		int a = 12;
	RL_Action nextAction = SafeNextAction(state);	

	//Start state
	if ( PreviousState[agentId] == nullState )
	{
		PreviousState[agentId] = state;
		PreviousAction[agentId] = nextAction;
		PreviousFrame[agentId] = game->frame;
		return nextAction;
	}
	//else continue

	//Reward
	float reward = 0;
	if (USE_RS_TIME)
	{
		reward = PreviousFrame[agentId] - game->frame;
	}


	float bestFutureValue;
	if ( state.IsTerminal() )
	{
		terminal = true;
		reward += 1000;
		bestFutureValue = reward;//no future actions to take
	}
	else
	{
		RL_Action bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction->GetValue(state, bestAction);
	}

	totalReward += reward;

	float value;
	//modify gamme according to use_qmsdp
	float gamma = (float)pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)game->frame - (double)PreviousFrame[agentId])));

	if(!USE_Q_LAMBDA)
	{
		//update own value function
		value = ValueFunction->GetValue(PreviousState[agentId],PreviousAction[agentId]) 
			+ ALPHA*(
			reward + gamma*bestFutureValue 
			- ValueFunction->GetValue(PreviousState[agentId],PreviousAction[agentId]) );

		ValueFunction->SetValue(PreviousState[agentId],PreviousAction[agentId], value);
	}
	else if(USE_Q_LAMBDA )
	{
		//add the current to the dataTrail

		//dataTrail.push_back(DataPoint(PreviousState[agentId], PreviousAction[agentId], state, reward, game->frame - PreviousFrame[agentId]));

		bool stateAndActionFound = false;
		/** ACCUMULATING TRACES START **/
		for ( int i = 0 ; i < (int)dataTrail.size() ; i++ )
		{
			if ( PreviousAction[agentId].ID == dataTrail[i].prevAction.ID &&
				PreviousState[agentId].GetID() == dataTrail[i].prevState.GetID()
			   )
			{
				dataTrail[i].eligibilityTrace += 1;
				stateAndActionFound = true;
			}
		}

		if (!stateAndActionFound)
		{
			dataTrail.push_back(DataPoint(PreviousState[agentId], PreviousAction[agentId], state, reward, game->frame - PreviousFrame[agentId]));
			//cerr << "DataTrail: " << dataTrail.size() << " Longest: " << LongestTrace << endl;
			if ( dataTrail.size() > LongestTrace )
			{
				LongestTrace = dataTrail.size();
				//cerr << "New Longest Trace: " << LongestTrace << endl;
			}
		}
		 /** ACCUMULATING TRACES END **/

		//Remove insignificant elements in the trace
		vector<DataPoint>::iterator it = dataTrail.begin();
		while ( it != dataTrail.end() )
		{
			if( it->eligibilityTrace < Q_LAMBDA_THRESHOLD)
				it = dataTrail.erase(it);
			else it++;
		}

		float delta = reward + gamma*bestFutureValue - ValueFunction->GetValue( PreviousState[agentId], PreviousAction[agentId] );

		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			value = ValueFunction->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
				+ ALPHA*delta*dataTrail[i].eligibilityTrace;

			ValueFunction->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);

			if ( m_greedyChoice )
			{
				dataTrail[i].eligibilityTrace *= gamma*LAMBDA;
			}
			else 
			{	
				dataTrail[i].eligibilityTrace = 0;
			}
		}
	}

	if ( terminal )
	{
		goalAchieved = true;
		return nullAction;//MEANS THAT YOU SHOULD STOP NOW!!
	}
	else
	{
		PreviousState[agentId] = state;
		PreviousAction[agentId] = nextAction;
		PreviousFrame[agentId] = game->frame;

		return nextAction;
	}
}

float RL::GetTotalReward()
{
	return totalReward;
}

