#include "RL.h"

int RUNS;
int NUM_LEARNERS;
float GAMMA;
float ALPHA;
float EPSILON_START;
float EPSILON_DECAY;
bool PRINT_REWARD;
bool USE_QSMDP;
bool USE_RS_TERMINATION;
bool USE_RS_TIME;
bool USE_Q_LAMBDA;

using namespace brainSpace;
using namespace std;

RL::RL(Game *g, double epsilon, int numAgents, bool load)
{	
	EPSILON = epsilon;
	game = g;
	nullState = RL_State();
	nullAction = RL_Action( -1, -1 );
	vector<QStateVar> stateVars;
	vector<QAction> actions;
	totalReward = 0.0;

	isTerminated = new bool[NUM_LEARNERS];
	for ( unsigned int i = 0; i < NUM_LEARNERS ; i++ ) 
	{
		isTerminated[i] = false;
	}
	lastTerminationReward = 0.0;
	lastReward = 0.0;

	for(int i = 0; i<numAgents; i++)
	{
		PreviousFrame.push_back(0);
		PreviousState.push_back(nullState);
		PreviousAction.push_back(nullAction);
	}
int bla;
	switch(RL_TYPE)
	{
	case 0:
		stateVars.push_back( QStateVar("ConCur", (int)pow((double)(3*5+1),NUM_LEARNERS-1) ) );//3 action * 5 time states + 1 null actions
		//stateVars.push_back( QStateVar("ConCurTime", 5));
		stateVars.push_back( QStateVar("MStore", 4));
		stateVars.push_back( QStateVar("EStore", 4));
		stateVars.push_back( QStateVar("MIncome", 4));
		stateVars.push_back( QStateVar("EIncome", 4));
		actions.push_back( QAction("Lab", 0));
		actions.push_back( QAction("Solar", 1));
		actions.push_back( QAction("Mex", 2));

		ValueFunction = new RL_Q( actions, stateVars);
		RL_State::lastLabCount = 0;//static from .h file
		break;
	default:
		break;
	}

	numActions = actions.size();
	numStates = 1;
	for ( unsigned int i = 0; i < stateVars.size() ; i++ )
	{
		numStates *= stateVars[i].numStates;
	}

	dataTrail.clear();
	LoadFromFile(load);
	goalAchieved = false;
}

RL::~RL()
{
	SaveToFile();
	delete ValueFunction;
	dataTrail.clear();
	delete isTerminated;
}

void RL::LoadFromFile(bool doIt)
{
	if (doIt)
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
				ValueFunction->LoadFromFile(readFile,doIt);
			}
			delete readFile;
		}
		delete[] path;
	}
}

char* RL::GetFilePath()
{
	const char* dir = RL_FILE_PATH;

	char *path = new char[200];
	strcpy(path, dir);

	switch(RL_TYPE)
	{
	case 0:
		strcat(path, RL_FILE_NO_END);
		break;
	default:
		break;
	}

	return path;

}

void RL::SaveToFile(bool doIt)
{
	if (doIt)
	{
		char *path = GetFilePath();

		ofstream *file = new ofstream(path, ios::binary | ios::out);

		FileHeader fileHeader;

		fileHeader.header[0] = FILE_HEADER[0];
		fileHeader.header[1] = FILE_HEADER[1];
		fileHeader.type = QBFILE_VERSION;
		fileHeader.numQTables = 1;

		file->write( (char*)&fileHeader, sizeof(fileHeader) );
		file->flush();

		ValueFunction->SaveToFile(file,doIt);

		file->flush();
		file->close();
		delete[] path;
		delete file;
	}
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
	double bestValue = ValueFunction->GetValue(state, action);

	for ( int i = 1 ; i < (int)stateActions.size() ; i++ )
	{
		RL_Action tempAction = stateActions[i];
		double tempValue = ValueFunction->GetValue(state, tempAction);

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
	RL_Action nextAction = SafeNextAction(state);	

	LastActionID = nextAction.ID;
	LastStateID = state.GetID();

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
	float reward = 0.0;
	if (USE_RS_TIME)
	{
		reward = (float)(PreviousFrame[agentId] - game->frame);
	}

	if (USE_RS_TERMINATION && state.IsTerminal() /*PreviousAction[agentId].Action == LAB_ID*/)
	{
		bool firstReward = true;
		for(int i = 0; i<NUM_LEARNERS; i++)
		{
			if (isTerminated[i])
			{
				firstReward = false;
				break;
			}
		}
		isTerminated[agentId] = true;
		float value;
		if (!COMMON_TERMINATION_REWARD || firstReward)//If we already calculated reward for this ternmination, use that
		{
			double metalGain = game->GetTotalProduction(MEX_ID);
			//metalGain -= game->GetResourceUsage(MEX_ID); test
			metalGain -= game->units[LAB_ID]*5;
			double energyGain = game->GetTotalProduction(SOLAR_ID);
			energyGain -= game->GetResourceUsage(SOLAR_ID);
			energyGain -= game->units[LAB_ID]*50;

			float metalValue = (float)((max(REWARD_METAL_MIN, min(REWARD_METAL_MAX, metalGain ) ) - REWARD_METAL_MIN ) / (REWARD_METAL_MAX-REWARD_METAL_MIN)); //metal production-usage [-1;1]
			float energyValue = (float)((max(REWARD_ENERGY_MIN, min(REWARD_ENERGY_MAX, energyGain ) ) - REWARD_ENERGY_MIN ) / (REWARD_ENERGY_MAX-REWARD_ENERGY_MIN)); //energy production-usage [-1;1]
			value = min(metalValue, energyValue);
			value *= 100;
			value += (float)( min( game->resources[MEX_ID]/10.0, game->resources[SOLAR_ID]/10.0 ) );
			lastTerminationReward = value;
		}
		else
		{
			value = lastTerminationReward;
		}

		//value = 1;
		reward += value;
		
		//cerr << "Termination reward:" << game->GetTotalProduction(MEX_ID) << ", " << game->GetUsage(MEX_ID) << " : " << game->GetTotalProduction(SOLAR_ID) << ", " << game->GetUsage(SOLAR_ID) << " => " << value << endl;
	}

	double bestFutureValue;
	if ( state.IsTerminal() )
	{
		terminal = true;
		bestFutureValue = reward;//no future actions to take
	}
	else
	{
		RL_Action bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction->GetValue(state, bestAction);
	}

	totalReward += reward;
	lastReward = reward;

	float value;
	//modify gamme according to use_qmsdp
	float gamma = (float)pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)game->frame - (double)PreviousFrame[agentId])));

	if(!USE_Q_LAMBDA)
	{
		//update own value function
		value = (float)(ValueFunction->GetValue(PreviousState[agentId],PreviousAction[agentId]) 
			+ ALPHA*(
			reward + gamma*bestFutureValue 
			- ValueFunction->GetValue(PreviousState[agentId],PreviousAction[agentId]) ));

		ValueFunction->SetValue(PreviousState[agentId],PreviousAction[agentId], value);
	}
	else if(USE_Q_LAMBDA )
	{
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState[agentId], PreviousAction[agentId], state, reward, (float)(game->frame - PreviousFrame[agentId])));

		for ( int i = 0 ; i < (int)dataTrail.size() ; i++ )
		{
			if( dataTrail[i].eligibilityTrace < Q_LAMBDA_THRESHOLD)
				dataTrail.erase(dataTrail.begin());
		}

		float delta = (float)(reward + gamma*bestFutureValue - ValueFunction->GetValue( PreviousState[agentId], PreviousAction[agentId] ));

		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			value = (float)ValueFunction->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
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
	//cerr << "value: " << value << endl;

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
float RL::GetLastReward()
{
	return lastReward;
}