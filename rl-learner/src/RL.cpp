
#include "RL.h"



using namespace brainSpace;
using namespace std;

RL::RL(Game *g, unsigned short int type, double epsilon)
{	
	EPSILON = epsilon;
	game = g;
	currentNode = 0;
	totalReward = 0;
	this->type = type;
	nullState = RL_State();
	nullAction = RL_Action( -1, -1 );
	vector<QStateVar> stateVars;
	vector<QAction> actions;

	switch(type)
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
	default:
		break;
	}
	
	ClearAllNodes();
	dataTrail.clear();

	PreviousFrame = 0;

	//Epsilon = 9;
	LoadFromFile();

	totalReward = 0.0;
	buildingToBuild = 0;
	goalAchieved = false;
}

RL::~RL()
{
	SaveToFile();
	delete ValueFunction;
	dataTrail.clear();
}

void RL::ClearAllNodes()
{
		PreviousState = nullState;
		PreviousAction = nullAction;
		ValueFunction->Clear();
}

void RL::LoadFromFile()
{
	const char* dir = "";

	char *path = new char[200];
	strcpy(path, dir);
	switch(type)
	{
	case 0:
		strcat(path, RL_FILE_1);
		break;
	default:
		break;
	}

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
			//for(int i = 0 ; i < fileHeader.numQTables; i++)
			//{
				ValueFunction->LoadFromFile(readFile);
			//}
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
	const char* dir = RL_FILE_PATH;
	
	char *path = new char[200];
	strcpy(path, dir);
	switch(type)
	{
	case 0:
		strcat(path, RL_FILE_1);
		break;
	default:
		break;
	}

	ofstream *file = new ofstream(path, ios::binary | ios::out);

	FileHeader fileHeader;

	fileHeader.header[0] = FILE_HEADER[0];
	fileHeader.header[1] = FILE_HEADER[1];
	fileHeader.numQTables = RL_NUM_NODES;
	fileHeader.type = QBFILE_VERSION;


	file->write( (char*)&fileHeader, sizeof(fileHeader) );

	//for(int i = 0; i< fileHeader.numQTables; i++)
	//{
		ValueFunction->SaveToFile(file);
	//}

	file->flush();
	file->close();
	delete[] path;
	delete file;
}

RL_State RL::GetState()
{
	return RL_State(game, type);
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
		game->greedy[0]++;
		
	}
	else //greedy
	{
		m_greedyChoice = true;
		action = FindBestAction(state);
		game->greedy[1]++;
	}
	return action;
}

RL_Action RL::FindBestAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();
	

	RL_Action action = stateActions[0]; //unitdefID
	
	float bestValue = ValueFunction->GetValue(state, action);

	

	//vector<RL_Action*>::iterator it;
	//for ( it = stateActions.begin()+1 ; it != stateActions.end() ; it++ )
	for ( int i = 1 ; i < (int)stateActions.size() ; i++ )
	{
		//RL_Action *tempAction = (RL_Action*)(*it);
		RL_Action tempAction = stateActions[i];
		float tempValue = ValueFunction->GetValue(state, tempAction);
		
		if ( tempValue > bestValue )
		{
			bestValue = tempValue;
			action = tempAction;
		}
	}
	if (action.ID < 0 || action.ID > 3)
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
		return nextAction;
	}
	
	return nullAction;
}

RL_Action RL::Update()
{
	
	bool terminal = false;
	RL_State state = GetState();
	RL_Action nextAction = SafeNextAction(state);
	
	
	//Start state
	if ( PreviousState == nullState )
	{
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = game->frame;
		return nextAction;
	}//else

	
	//Reward
	float reward;
	reward = 0;
	if (USE_RS_TIME) reward = PreviousFrame - game->frame;

	if(USE_RS_LABS && PreviousAction.Action == RL_LAB_ID)
		reward += 10;

	//cout << "previousframe: " << PreviousFrame[currentNode] << "\n";
	//cout << "real-frame: " << game->frame << "\n";
	//cout << "Reward: " << reward << "\n";
	float bestFutureValue;
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
	if(currentNode == 0)
	{
		//cout << "Adding reward: " << reward << ", new total: " << (totalReward+reward) << "\n";
		totalReward += reward;
	}

	float value;

	if(!USE_N_STEP && !USE_Q_LAMBDA)
	{
		//update own value function
		value = ValueFunction->GetValue(PreviousState,PreviousAction) 
				+ ALPHA*(
					reward + pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)game->frame - (double)PreviousFrame)))*bestFutureValue 
					- ValueFunction->GetValue(PreviousState,PreviousAction) );
		ValueFunction->SetValue(PreviousState,PreviousAction, value);
	}
	
	if(USE_BACKTRACKING)
	{
		//backtrack hack
		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			RL_Action bestAction = FindBestAction( dataTrail[i].resultState );
			float bestFutureValue = ValueFunction->GetValue(dataTrail[i].resultState, bestAction);
			value = ValueFunction->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
					+ ALPHA*(
						dataTrail[i].reward + pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*((0.01*(double)dataTrail[i].duration)))*bestFutureValue 
						- ValueFunction->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction) );
			ValueFunction->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);
		}
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState, PreviousAction, state, reward, game->frame - PreviousFrame));
		if(BACKTRACKING_STEPS > 0 &&  dataTrail.size() > BACKTRACKING_STEPS)
			dataTrail.erase(dataTrail.begin());
	}

	if(USE_Q_LAMBDA )
	{
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState, PreviousAction, state, reward, game->frame - PreviousFrame));
		
		for ( int i = 0 ; i < dataTrail.size() ; i++ )
			if( dataTrail[i].eligibilityTrace < Q_LAMBDA_THRESHOLD)
				dataTrail.erase(dataTrail.begin());

		//RL_Action bestAction = FindBestAction( state );
		float delta = reward + GAMMA*bestFutureValue - ValueFunction->GetValue( PreviousState, PreviousAction );

		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			value = ValueFunction->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
				+ ALPHA*delta*dataTrail[i].eligibilityTrace;
			ValueFunction->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);
			if ( m_greedyChoice )
			{
				dataTrail[i].eligibilityTrace *= GAMMA*LAMBDA;
			}
			else dataTrail[i].eligibilityTrace = 0;
		}
		
	}
	
	if(USE_N_STEP)
	{
		bool done = false;
		dataTrail.push_back(DataPoint(PreviousState, PreviousAction, state, reward, game->frame - PreviousFrame));
		while(!done)
		{
			float powerDiscount = 0.0;
			if(terminal || (BACKTRACKING_STEPS > 0 && dataTrail.size() > BACKTRACKING_STEPS))
			{
				float stepReward = 0.0;
				for(int i = 0; i<dataTrail.size(); i++)
				{
					stepReward += dataTrail[i].reward * pow((float)GAMMA, powerDiscount);
					if(USE_QSMDP)
						powerDiscount += 0.01*dataTrail[i].duration;
					else
						powerDiscount += 1;
				}
				//RL_Action bestAction = FindBestAction( dataTrail[i].resultState );
				stepReward += bestFutureValue; //ValueFunction[currentNode]->GetValue(dataTrail[i].resultState, bestAction);
				ValueFunction->SetValue(dataTrail[0].prevState, dataTrail[0].prevAction, stepReward);
				dataTrail.erase(dataTrail.begin());
			}
			if(dataTrail.size() > 0 && terminal)
				done = false;
			else
				done = true;
		}
	}

	if ( terminal )
	{
		goalAchieved = true;
		return nullAction;//MEANS THAT YOU SHOULD STOP NOW!!
	}
	else
	{
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = game->frame;
		
		return nextAction;
	}
}

float RL::GetTotalReward()
{
	return totalReward;
}

