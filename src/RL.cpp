#include "RL.h"
#include "RL_Q.h"
#include "Knowledge.h"

using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL(AIClasses* aiClasses, double epsilon, int numAgents)
{	
	ai = aiClasses;
	EPSILON = epsilon;
	//nullAction = RL_Action();
	//nullState = new RL_State();

	totalReward = 0.0f;
	PreviousFrame = 0;
	PreviousState = NULL;
	PreviousAction = NULL;

	switch(RL_TYPE)
	{
	case 0:
		{
			StateVars.push_back( QStateVar( "MexSpotCount", 3 ) );
			StateVars.push_back( QStateVar( "DistMexSpot", 4 ) );
			StateVars.push_back( QStateVar( "ImgMexSpotInf", 4 ) );
			StateVars.push_back( QStateVar( "AirGroup", 2 ) );
			StateVars.push_back( QStateVar( "GroupSpeed", 3 ) );
			StateVars.push_back( QStateVar( "SuperiorPathLength", 2 ) );
			StateVars.push_back( QStateVar( "CurrentSpotInf", 4 ) );

			Actions.push_back( QAction( "AttackMexSpot", 0 ) );

			ValueFunction = new RL_Q( ai, Actions, StateVars);
			break;
		}
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
	//ai->utility->ChatMsg("RL:About to delete valuefunc");
	delete ValueFunction;

	//ai->utility->ChatMsg("RL:About to clear datatrail");
	dataTrail.clear();
	//ai->utility->ChatMsg("RL:About to delete prevstate");
	delete PreviousState;
	//ai->utility->ChatMsg("RL: Deleted prevstate");
	//delete nullState;
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
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);

	switch(RL_TYPE)
	{
	case 0:
		{
			strcat(path, RL_FILE_ATTACK);
			break;
		}
	default:
		break;
	}
	delete dirs;
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
	fileHeader.numQTables = 1;

	file->write( (char*)&fileHeader, sizeof(fileHeader) );
	ValueFunction->SaveToFile(file);
	file->flush();
	file->close();
	delete[] path;
	delete file;
}


void RL::SaveToStateVisitsFile( int stateID )
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "visits.dat");

	fstream *file = new fstream();
	file->open(path, ios::in | ios::binary );


	//file did not exist!! Create it with all zeros!
	if ( !file->is_open() )
	{
		unsigned int maxState = 1;
		for ( unsigned int i = 0 ; i < StateVars.size() ; i++ )
		{
			maxState *= StateVars[i].numStates;
		}

		for ( unsigned int i = 0 ; i < Actions.size() ; i++ )
			maxState *= (i+1);

		file->close();
		file = new fstream();
		ai->utility->ChatMsg( "Making new file");
		file->open( path, ios::binary | ios::out );

		long a = file->tellp();
		ai->utility->ChatMsg( "p start: %d", a);

		for ( unsigned int i = 0 ; i < maxState ; i++ )
		{
			ai->utility->WriteToStateVisitFile( file, 0 );
		}

		a = file->tellp();
		ai->utility->ChatMsg( "p end: %d", a);
	}

	file->close();
	file->open( path, ios::out | ios::binary | ios::in );
	int d = ai->utility->ReadFromStateVisitFile( file, stateID );
	//ai->utility->ChatMsg("RL: Old visits to %d. %d", stateID, d );
	ai->utility->WriteToStateVisitFile( file, d+1, stateID );
	ai->utility->ChatMsg("RL: New visits to %d. %d", stateID, ai->utility->ReadFromStateVisitFile(file, stateID ) );
	file->close();
	delete dirs;
	delete[] path;
	delete file;
}


RL_State* RL::GetState(MilitaryUnitGroup* group, vector<pair<int, SAIFloat3> > resourceBuildings )
{
	return new RL_State(ai, group, StateVars, resourceBuildings, ValueFunction );
}

RL_Action* RL::FindNextAction( RL_State &state )
{
	vector<RL_Action*> stateActions = state.GetActions();
	RL_Action* action = stateActions[0]; //unitdefID

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

RL_Action* RL::FindBestAction( RL_State &state )
{
	vector<RL_Action*> stateActions = state.GetActions();
	RL_Action* action = stateActions[0]; //unitdefID
	float bestValue = ValueFunction->GetValue(state, *action);

	for ( unsigned int i = 1 ; i < stateActions.size() ; i++ )
	{
		RL_Action* tempAction = stateActions[i];
		float tempValue = ValueFunction->GetValue(state, *tempAction);

		if ( tempValue > bestValue )
		{
			bestValue = tempValue;
			action = tempAction;
		}
	}

	return action;
}

RL_Action* RL::SafeNextAction(RL_State &state)
{
	if(state.GetActions().size() > 0)
	{
		return FindNextAction( state );
	}	
	return NULL;
}

RL_Action* RL::Update(MilitaryUnitGroup* group)
{
	bool terminal = false;
	vector<Unit*> units = ai->callback->GetEnemyUnits();
	vector< pair<int, SAIFloat3> > mexPositions;
	vector< pair<int, SAIFloat3> > solarPositions;
	vector< pair<int, SAIFloat3> > windGenePositions;

	for ( unsigned int i = 0 ; i < units.size() ; i++ )
	{
		UnitDef* d = units[i]->GetDef();
		if ( strcmp( "armmex", d->GetName() ) == 0 )
			mexPositions.push_back( make_pair( units[i]->GetUnitId(), units[i]->GetPos() ) );
		else if ( strcmp( "armsolar", d->GetName() ) == 0 )
			solarPositions.push_back( make_pair( units[i]->GetUnitId(), units[i]->GetPos() ) );
		else if ( strcmp( "armwin", d->GetName() ) == 0 )
			windGenePositions.push_back( make_pair( units[i]->GetUnitId(), units[i]->GetPos() ) );
		
		delete d;
		delete units[i];
	}


	RL_State* state = NULL;

	RL_Action* nextAction = NULL;

	if ( group != NULL )
	{
		RL_State* mexState = NULL;
		RL_State* solarState = NULL;
		RL_State* windState = NULL;
		int chosenOne = 0;

		if ( mexPositions.size() > 0 )
			mexState = GetState( group, mexPositions );
		if ( solarPositions.size() > 0 )
			solarState = GetState( group, solarPositions );
		if ( windGenePositions.size() > 0 )
			windState = GetState( group, windGenePositions );

		double bestVal = -1000000000;

		if ( mexState != NULL && mexState->GetID() != -1 )
		{
			if ( mexState->ExpectedReward > bestVal )
			{
				bestVal = mexState->ExpectedReward;
				state = mexState;
			}
		}
		if ( solarState != NULL && solarState->GetID() != -1 )
		{
			if ( solarState->ExpectedReward > bestVal )
			{
				bestVal = solarState->ExpectedReward;
				state = solarState;
				chosenOne = 1;
			}
		}

		if ( windState != NULL && windState->GetID() != -1 )
		{
			if ( windState->ExpectedReward > bestVal )
			{
				bestVal = windState->ExpectedReward;
				state = windState;
				chosenOne = 2;
			}
		}

		nextAction = SafeNextAction(*state);
		if ( state != NULL )
		{
			switch(chosenOne)
			{
			case 0:
				{
					if ( solarState != NULL )
						delete solarState;
					if ( windState != NULL )
						delete windState;
					break;
				}
			case 1:
				{
					if ( mexState != NULL )
						delete mexState;
					if ( windState != NULL )
						delete windState;
					break;
				}
			case 2:
				{	
					if ( mexState != NULL )
						delete mexState;
					if ( solarState != NULL )
						delete solarState;
					break;
				}
			}
		}
	}

	//Start state
	if ( PreviousState == NULL )
	{
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = ai->frame;
		if ( state != NULL && state->GetID() != -1 )
		{
			//ai->utility->ChatMsg("RL: About to do something unsafe!");
			//ai->utility->ChatMsg("RL: Done something unsafe!");
		}
		return nextAction;
	}
	//else continue

	//ai->utility->ChatMsg("RL: about to calculate reward");
	//Reward
	double reward = 0.0;
	if (USE_RS_TIME)
	{
		reward = PreviousFrame - ai->frame;
	}
	//ai->utility->ChatMsg("PreviousAction unitIDs: %d", PreviousAction->unitIDs.size() );


	map<int, UnitInformationContainer> existingUnits = ai->knowledge->enemyInfo->baseInfo->GetUnits();
	map<int, UnitInformationContainer> ourUnits = ai->knowledge->selfInfo->armyInfo->GetUnits();
	unsigned int mexKilledCounter = 0;
	float unitLossReward = 0.0f;

	float totalHealth = 0.0f;
	float totalRemainingHealth = 0.0f;

	for ( unsigned int i = 0 ; i < PreviousAction->unitIDs.size() ; i++ )
	{
		if ( existingUnits.find(PreviousAction->unitIDs[i] ) == existingUnits.end() )
			mexKilledCounter++;
	}
	if ( group != NULL )
	{
		for ( int i = 0 ; i < group->GetSize() ; i++ )
		{
			Unit* u = Unit::GetInstance(ai->callback, group->GetUnits()[i] );

			UnitDef* d = u->GetDef();
			float defHealth = d->GetHealth();
			float unitHealth = u->GetHealth();
			if ( unitHealth > defHealth )
				unitHealth = defHealth;

			totalHealth += defHealth;
			totalRemainingHealth += unitHealth;
			
			delete d;
			delete u;
		}
		unitLossReward = (totalRemainingHealth/totalHealth);
	}
	else 
	{
		//all units killed
		unitLossReward = 0;
	}

	float bestFutureValue = 0.0f;
	if ( state != NULL )
	{
		if ( state->IsTerminal() )
		{
			terminal = true;
			bestFutureValue = reward;//no future actions to take
		}
		else
		{
			RL_Action* bestAction = FindBestAction( *state );
			bestFutureValue = ValueFunction->GetValue(*state, *bestAction);
		}
	}

	if ( PreviousAction->unitIDs.size() == mexKilledCounter )
	{
		reward = 100;
	}
	else
	{
		reward = mexKilledCounter/PreviousAction->unitIDs.size()*100;
	}

	reward *= unitLossReward;

	//ai->utility->ChatMsg("Reward is %f", reward );
	totalReward += reward;

	double value;
	//modify gamme according to use_qmsdp
	double gamma = (float)pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)ai->frame - (double)PreviousFrame)));

	if(!USE_Q_LAMBDA)
	{
		//update own value function
		value = ValueFunction->GetValue(*PreviousState,*PreviousAction);
		//	+ ALPHA*(
		//	reward + gamma*bestFutureValue 
		//	- ValueFunction->GetValue(*PreviousState,*PreviousAction) );
		//ai->utility->ChatMsg("Old Value: %f", value );
		SaveToStateVisitsFile( PreviousState->GetID() );
		ValueFunction->SetValue(*PreviousState,*PreviousAction, value+reward);
		return NULL;
	}
	else if(USE_Q_LAMBDA )
	{
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(*PreviousState, *PreviousAction, *state, reward, ai->frame - PreviousFrame));

		for ( unsigned int i = 0 ; i < dataTrail.size() ; i++ )
		{
			if( dataTrail[i].eligibilityTrace < Q_LAMBDA_THRESHOLD)
				dataTrail.erase(dataTrail.begin());
		}

		double delta = reward + gamma*bestFutureValue - ValueFunction->GetValue( *PreviousState, *PreviousAction );

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
		return NULL;//MEANS THAT YOU SHOULD STOP NOW!!
	}
	else
	{
		delete PreviousState;
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = ai->frame;

		return nextAction;
	}
}

float RL::GetTotalReward()
{
	return totalReward;
}


bool RL::MayUpdate()
{
	if ( PreviousAction == NULL )
		return false;

	//Group was killed
	if ( PreviousAction->Group == NULL )
	{
		//ai->utility->ChatMsg("MayUpdate: Group NULL");	
		return true;
	}
	bool allUnitsKilled = true;
	map<int, UnitInformationContainer> existingUnits = ai->knowledge->enemyInfo->baseInfo->GetUnits();

	for ( unsigned int i = 0 ; i < PreviousAction->unitIDs.size() ; i++ )
	{
		if ( existingUnits.find(PreviousAction->unitIDs[i] ) != existingUnits.end() )
			allUnitsKilled = false;
	}

	if ( allUnitsKilled )
	{
		//ai->utility->ChatMsg("MayUpdate: All units killed");
		return true;
	}
	else
	{
		//ai->utility->ChatMsg("MayUpdate: All units not killed");
		return false;
	}
}

void RL::SetMayUpdate(bool mayUpdate)
{
	MayUpdateVar = mayUpdate;
}