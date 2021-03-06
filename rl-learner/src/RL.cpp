#include "RL.h"

int RUNS;
int NUM_LEARNERS;
float GAMMA;
float ALPHA;
double EPSILON_START;
double EPSILON_DECAY;
bool PRINT_REWARD;
bool USE_QSMDP;
bool USE_RS_TERMINATION;
bool USE_RS_TIME;
bool USE_Q_LAMBDA;
int CONCURRENT_I;
int CONCURRENT_T;
int CONCURRENT_SS;
bool USE_NEW_REWARD_CODE;

using namespace brainSpace;
using namespace std;

int factorial(int n)
{
	int result = 1;
	for(int i = 1; i <= n; i++)
	{
		result *= i;
	}
	return result;
	//if (n<=1)
	//	return(1);
	//else
	//	n=n*factorial(n-1);
	//return(n);
}

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
	for ( int i = 0; i < NUM_LEARNERS ; i++ ) 
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
		vector<pair<double,double>> v;
		TempReward.push_back(v);
	}

	switch(RL_TYPE)
	{
	case 0:
		switch (CONCURRENT_SS) {
			case 1:
				stateVars.push_back( QStateVar("ConCur", (int)pow((double)(CONCURRENT_A+1),(int)NUM_LEARNERS-1) ) );//3 action * 5 time states + 1 null actions
				break;
			case 2:
				stateVars.push_back( QStateVar("ConCur", (int)pow((double)(CONCURRENT_A*CONCURRENT_T+1),(int)NUM_LEARNERS-1) ) );//3 action * 5 time states + 1 null actions
				break;
			case 3:
				/*
				pascal(n,k) &= \frac{k!}{n!(n-k)!} \\
				f3(a,n) &= pascal(n+a-1, a)
				*/
				stateVars.push_back( QStateVar("ConCur", factorial(NUM_LEARNERS+CONCURRENT_A-1)/(factorial(CONCURRENT_A)*factorial(NUM_LEARNERS-1)) ) );
				break;
			case 4:
				stateVars.push_back( QStateVar("ConCur", (int)pow((float)CONCURRENT_I, (int)CONCURRENT_A) ) );
				break;
		}
		//stateVars.push_back( QStateVar("ConCurTime", 5));
#ifdef USE_BUILDING_COUNT
		stateVars.push_back( QStateVar("SOLAR", RL_SOLAR_INDEX));
		stateVars.push_back( QStateVar("MEX", RL_MEX_INDEX));
		stateVars.push_back( QStateVar("LAB", RL_LAB_INDEX));
#else
		stateVars.push_back( QStateVar("MStore", REWARD_METAL_STORE_STATES));
		stateVars.push_back( QStateVar("EStore", REWARD_ENERGY_STORE_STATES));
		stateVars.push_back( QStateVar("MIncome", REWARD_METAL_STATES));
		stateVars.push_back( QStateVar("EIncome", REWARD_ENERGY_STATES));
#endif
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

double RL::GetReward()
{
	double value;
	double metalGain = game->GetTotalProduction(MEX_ID);
	//metalGain -= game->GetResourceUsage(MEX_ID);
	metalGain -= game->units[LAB_ID]*5;

	double energyGain = game->GetTotalProduction(SOLAR_ID);
	energyGain -= game->GetResourceUsage(SOLAR_ID);
	energyGain -= game->units[LAB_ID]*50;
	
	float metalValue = (float)((max(REWARD_METAL_MIN, min(REWARD_METAL_MAX, metalGain ) ) - REWARD_METAL_MIN ) / (REWARD_METAL_MAX-REWARD_METAL_MIN)); //metal production-usage [-1;1]
	float energyValue = (float)((max(REWARD_ENERGY_MIN, min(REWARD_ENERGY_MAX, energyGain ) ) - REWARD_ENERGY_MIN ) / (REWARD_ENERGY_MAX-REWARD_ENERGY_MIN)); //energy production-usage [-1;1]
	value = min(metalValue, energyValue);
	value *= 100;
	value += ( min( game->resources[MEX_ID]/10.0, game->resources[SOLAR_ID]/10.0 ) );

	
	return value;
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
	double reward = 0.0;
	if (USE_RS_TIME)
	{
		double time = (double)(PreviousFrame[agentId] - game->frame);
		if(USE_QSMDP)
		{
			double gamma = pow((double)GAMMA, 0.01*((double)game->frame - (double)PreviousFrame[agentId]));
			reward += time*gamma;
		}
		else
		{
			reward += time;
		}
	}

	if (USE_NEW_REWARD_CODE)
	{
		//new reward code 
		if(PreviousAction[agentId].Action == LAB_ID && game->units[LAB_ID] <= RL_LAB_INDEX)
		{
			double value = GetReward();
			for(int i = 0; i < NUM_LEARNERS; i++)
			{
				pair<double,double> p;
				p.first = value;
				p.second = game->frame;
				TempReward[i].push_back(p);
			}
		}
		assert(COMMON_TERMINATION_REWARD);
		for(int i = 0; i < TempReward[agentId].size();i++)
		{
			double gamma = 1.0;
			if(USE_QSMDP)
			{
				gamma = pow((double)GAMMA, 0.01*((double)TempReward[agentId][i].second - (double)PreviousFrame[agentId]));
			}
			reward += TempReward[agentId][i].first*gamma;
		}
		TempReward[agentId].clear();
	}
	else
	{
	
		//old reward code
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
			double value;
			if (!COMMON_TERMINATION_REWARD || firstReward)//If we already calculated reward for this ternmination, use that
			{
				value = GetReward();
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
	}
	
	double bestFutureValue;
	if ( state.IsTerminal() )
	{
		terminal = true;
		bestFutureValue = 0;//reward;//no future actions to take
	}
	else
	{
		RL_Action bestAction = FindBestAction( state );
		bestFutureValue = ValueFunction->GetValue(state, bestAction);
	}

	totalReward += reward;
	lastReward = reward;

	double value;
	//modify gamma according to use_qmsdp
	double gamma = pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)game->frame - (double)PreviousFrame[agentId])));

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
		//remove any doubles before adding a new
		for ( int i = 0 ; i < (int)dataTrail.size() ; i++ )
		{
			if( dataTrail[i].prevState == PreviousState[agentId] && dataTrail[i].prevAction == PreviousAction[agentId] )
			{
				dataTrail.erase(dataTrail.begin()+i);
				break;
			}
		}
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState[agentId], PreviousAction[agentId], state, reward, (float)(game->frame - PreviousFrame[agentId])));

		for ( int i = 0 ; i < (int)dataTrail.size() ; i++ )
		{
			if( dataTrail[i].eligibilityTrace < Q_LAMBDA_THRESHOLD)
				dataTrail.erase(dataTrail.begin());
		}

		double delta = reward + gamma*bestFutureValue - ValueFunction->GetValue( PreviousState[agentId], PreviousAction[agentId] );

		for (int i = dataTrail.size()-1; i>=0; i--)
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

double RL::GetTotalReward()
{
	return totalReward;
}
double RL::GetLastReward()
{
	return lastReward;
}