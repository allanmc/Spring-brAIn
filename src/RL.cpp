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
	CurrentQTable = -1;

	switch(RL_TYPE)
	{
	case 0:
		{
			StateVars.resize(3);
			Actions.resize(3);
			ValueFunction.resize(3);

			StateVars[0].push_back( QStateVar( "MexSpotCount", 3 ) );
			StateVars[0].push_back( QStateVar( "DistMexSpot", 4 ) );
			StateVars[0].push_back( QStateVar( "ImgMexSpotInf", 4 ) );
			StateVars[0].push_back( QStateVar( "GroupSpeed", 3 ) );
			//StateVars[0].push_back( QStateVar( "SuperiorPathLength", 2 ) );
			StateVars[0].push_back( QStateVar( "CurrentSpotInf", 4 ) );

			Actions[0].push_back( QAction( "AttackMexSpot", 0 ) );

			ValueFunction[0] = new RL_Q( ai, Actions[0], StateVars[0]);


			/************************/
			StateVars[1].push_back( QStateVar( "SolarSpotCount", 3 ) );
			StateVars[1].push_back( QStateVar( "DistSolarSpot", 4 ) );
			StateVars[1].push_back( QStateVar( "ImgSolarSpotInf", 4 ) );
			StateVars[1].push_back( QStateVar( "GroupSpeed", 3 ) );
			//StateVars[1].push_back( QStateVar( "SuperiorPathLength", 2 ) );
			StateVars[1].push_back( QStateVar( "CurrentSpotInf", 4 ) );

			Actions[1].push_back( QAction( "AttackSolarSpot", 0 ) );

			ValueFunction[1] = new RL_Q( ai, Actions[1], StateVars[1]);


			/************************/
			StateVars[2].push_back( QStateVar( "WGenCount", 3 ) );
			StateVars[2].push_back( QStateVar( "DistWGen", 4 ) );
			StateVars[2].push_back( QStateVar( "ImgWGenInf", 4 ) );
			StateVars[2].push_back( QStateVar( "GroupSpd", 3 ) );
			//StateVars[2].push_back( QStateVar( "SupPath", 2 ) );
			StateVars[2].push_back( QStateVar( "CurSpotInf", 4 ) );

			Actions[2].push_back( QAction( "AttackWindSpot", 0 ) );

			ValueFunction[2] = new RL_Q( ai, Actions[2], StateVars[2]);
			break;
		}
	default:
		break;
	}

	dataTrail.clear();
	for ( int i = 0 ; i < NUM_Q_TABLES ; i++ )
		LoadFromFile(i);	
	goalAchieved = false;
}

RL::~RL()
{
	//ai->utility->ChatMsg("RL:About to savetofile");
	for ( int i = 0 ; i < NUM_Q_TABLES ; i++ )
	{
		//ai->utility->ChatMsg("SAVING FILE %d", i );
		SaveToFile(i);
	}
	//ai->utility->ChatMsg("RL:About to delete valuefunc");
	for (int i = 0 ; i < 3 ; i++ )
	{
		delete ValueFunction[i];
		ValueFunction[i] = NULL;
	}

	//ai->utility->ChatMsg("RL:About to clear datatrail");
	dataTrail.clear();
	//ai->utility->ChatMsg("RL:About to delete prevstate");
	if ( PreviousState != NULL )
	{
		delete PreviousState;
		PreviousState = NULL;
	}
	//ai->utility->ChatMsg("RL: Deleted prevstate");
	//delete nullState;
}

void RL::LoadFromFile(int type)
{
	char *path = GetFilePath(type);

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
			ValueFunction[type]->LoadFromFile(readFile);
		}
		delete readFile;
		readFile = NULL;
	}

	delete[] path;
}

char* RL::GetFilePath(int type)
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);

	switch(RL_TYPE)
	{
	case 0:
		{
			switch( type )
			{
			case 0:
				strcat(path, RL_FILE_ATTACK_MEX);
				break;
			case 1:
				strcat(path, RL_FILE_ATTACK_SOL);
				break;
			case 2:
				strcat(path, RL_FILE_ATTACK_WIN);
				break;
			}
			break;
		}
	default:
		break;
	}
	delete dirs;
	dirs = NULL;
	return path;

}

void RL::SaveToFile(int type)
{
	char *path = GetFilePath(type);

	ofstream *file = new ofstream(path, ios::binary | ios::out);

	FileHeader fileHeader;

	fileHeader.header[0] = FILE_HEADER[0];
	fileHeader.header[1] = FILE_HEADER[1];
	fileHeader.type = QBFILE_VERSION;
	fileHeader.numQTables = 1;

	file->write( (char*)&fileHeader, sizeof(fileHeader) );
	ValueFunction[type]->SaveToFile(file);
	file->flush();
	file->close();
	delete[] path;
	delete file;
	file = NULL;
}


RL_State* RL::GetState(MilitaryUnitGroup* group, vector<pair<int, SAIFloat3> > resourceBuildings, int type )
{
	return new RL_State(ai, group, StateVars[type], resourceBuildings, ValueFunction[type], Epsilon, type );
}

RL_Action* RL::FindNextAction( RL_State *state, int type )
{
	vector<RL_Action*> stateActions = state->GetActions();
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
		action = FindBestAction(state, type);
	}
	return action;
}

RL_Action* RL::FindBestAction( RL_State *state, int type )
{
	vector<RL_Action*> stateActions = state->GetActions();
	RL_Action* action = stateActions[0]; //unitdefID
	float bestValue = ValueFunction[type]->GetValue(*state, *action);

	for ( unsigned int i = 1 ; i < stateActions.size() ; i++ )
	{
		RL_Action* tempAction = stateActions[i];
		float tempValue = ValueFunction[type]->GetValue(*state, *tempAction);

		if ( tempValue > bestValue )
		{
			bestValue = tempValue;
			action = tempAction;
		}
	}

	return action;
}

RL_Action* RL::SafeNextAction(RL_State* state, int type )
{
	if ( state != NULL )
	{
		vector<RL_Action*> a = state->GetActions();
		if(a.size() > 0)
		{
			return FindNextAction( state, type );
		}
	}
	return NULL;
}

RL_Action* RL::Update(MilitaryUnitGroup* group)
{
	ai->utility->ChatMsg("RL: UPDATE");
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
		d = NULL;
		delete units[i];
		units[i] = NULL;
	}


	RL_State* state = NULL;
	RL_Action* nextAction = NULL;

	if ( group != NULL )
	{
		if ( PreviousState == NULL )
		{
			ai->utility->ChatMsg("RL:Finding new state");
			vector<RL_State*> states;
			RL_State* mexState = NULL;
			RL_State* solarState = NULL;
			RL_State* windState = NULL;

			if ( mexPositions.size() > 0 )
			{
				//ai->utility->ChatMsg("RL:Getting mexstate");
				mexState = GetState( group, mexPositions, 0 );
				if ( mexState != NULL && mexState->GetID() != -1 )
					states.push_back(mexState);
			}
			if ( solarPositions.size() > 0 )
			{
				//ai->utility->ChatMsg("RL:Getting solarstate");
				solarState = GetState( group, solarPositions, 1 );
				if ( solarState != NULL && solarState->GetID() != -1 )
					states.push_back(solarState);
			}
			if ( windGenePositions.size() > 0 )
			{
				//ai->utility->ChatMsg("RL:Getting windstate");
				windState = GetState( group, windGenePositions, 2 );
				if ( windState != NULL && windState->GetID() != -1 )
					states.push_back(windState);
			}

			double bestVal = -1000000000;
			int index = -1;
			if ( states.size() > 0 )
			{
				//ai->utility->ChatMsg("RL: %d different resource types", states.size() );
				int fewestVists = 1000000000;
				for ( int i = 0 ; i < states.size() ; i++ )
				{
					int v = states[i]->GetVisitsTo( states[i]->GetID(), states[i]->Type );
					if ( v < fewestVists )
					{
						fewestVists = v;
						index = i;
					}
				}
				state = states[index];
				CurrentQTable = state->Type;
				//ai->utility->ChatMsg("CurrentQ: %d. StateType: %d. ID: %d", CurrentQTable, state->Type, state->GetID() );
				//ai->utility->ChatMsg("Old Visits to %d in table %d: %d", state->GetID(), state->Type, state->GetVisitsTo(state->GetID(), state->Type ));
			}
			else return NULL;

			for ( int i = 0 ; i < states.size() ; i++ )
			{
				if ( i == index ) continue;
				delete states[i];
				states[i] = NULL;
			}

			nextAction = SafeNextAction(state);
			ai->utility->ChatMsg("RL Update currentQTable: %d. State: %d", CurrentQTable, state->GetID() );
		}
	}

	//Start state
	if ( PreviousState == NULL )
	{
		//ai->utility->ChatMsg("RL:PrevState NULL");
		PreviousState = state;
		PreviousAction = nextAction;
		PreviousFrame = ai->frame;
		return nextAction;
	}

	if ( PreviousAction == NULL )
	{
		//ai->utility->ChatMsg("RL: PRevAction NULL");
		return NULL;
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
	unsigned int buildingsKilledCounter = 0;
	float unitLossReward = 0.0f;

	float totalHealth = 0.0f;
	float totalRemainingHealth = 0.0f;

	vector<Unit*> thisWorks = ai->callback->GetEnemyUnits();
	for ( unsigned int i = 0 ; i < PreviousAction->unitIDs.size() ; i++ )
	{
		bool foundInExisitng = false;
		for ( unsigned int j = 0 ; j < thisWorks.size() ; j++ )
		{
			//ai->utility->ChatMsg("UnitIDs %d. ExisitngUnit %d", PreviousAction->unitIDs[i], thisWorks[j]->GetUnitId() );
			if ( thisWorks[j]->GetUnitId() == PreviousAction->unitIDs[i] )
			{
				foundInExisitng = true;
				break;
			}
		}
		if ( !foundInExisitng )
		{
			//ai->utility->ChatMsg("%d not found in exisiting", PreviousAction->unitIDs[i] );
			buildingsKilledCounter++;
		}
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
			d = NULL;
			delete u;
			u = NULL;
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
			RL_Action* bestAction = FindBestAction( state );
			if ( CurrentQTable != -1 )
				bestFutureValue = ValueFunction[CurrentQTable]->GetValue(*state, *bestAction);
			else ai->utility->ChatMsg("RL: CRAP!");
		}
	}


	ai->utility->ChatMsg("Buildingskilledcounter: %d. UnitIDs: %d", buildingsKilledCounter, PreviousAction->unitIDs.size() );

	reward = ((float)buildingsKilledCounter/(float)PreviousAction->unitIDs.size())*100.0f;

	//ai->utility->ChatMsg("Reward %f", reward );
	if ( buildingsKilledCounter > 3 ) buildingsKilledCounter = 3;

	reward *= (float)buildingsKilledCounter/(float)3;//make killing 3 mexes count more than killing 2 mexes
	ai->utility->ChatMsg("Reward after *= buildingsKilledCounter %f", reward );
	reward *= unitLossReward;//discount according to unitdamage
	ai->utility->ChatMsg("Reward after *= unitLossReward %f", reward );

	//ai->utility->ChatMsg("Reward is %f", reward );
	totalReward += reward;

	double value;
	//modify gamme according to use_qmsdp
	double gamma = (float)pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)ai->frame - (double)PreviousFrame)));


	if(!USE_Q_LAMBDA)
	{
		//update own value function
		if ( CurrentQTable != -1 )
		{
			value = ValueFunction[CurrentQTable]->GetValue(*PreviousState,*PreviousAction);
			//non-visited state has negative reward to indicate that they are non-visited.
			if ( value < 0 )
				value = 0;
			//	+ ALPHA*(
			//	reward + gamma*bestFutureValue 
			//	- ValueFunction[CurrentQTable]->GetValue(*PreviousState,*PreviousAction) );
			ValueFunction[CurrentQTable]->SetValue(*PreviousState,*PreviousAction, value+reward);
			ai->utility->ChatMsg("---");
			ai->utility->ChatMsg("Value: %f. QTable: %d. State: %d", ValueFunction[CurrentQTable]->GetValue(*PreviousState, *PreviousAction), CurrentQTable, PreviousState->GetID() );
			ai->utility->ChatMsg("---");
			PreviousState->UpdateVisitsTo( PreviousState->GetID(), PreviousState->Type );
			char buf[200];
			sprintf( buf, "New visits to %d in table %d: %d\n", PreviousState->GetID(), PreviousState->Type, PreviousState->GetVisitsTo(PreviousState->GetID(), PreviousState->Type ));
			ai->utility->ChatMsg("New visits to %d in table %d: %d", PreviousState->GetID(), PreviousState->Type, PreviousState->GetVisitsTo(PreviousState->GetID(), PreviousState->Type ));
			ofstream of;
			of.open( "RL-LOGFILE.txt", ios::out | ios::app );
			of.write( buf, strlen(buf) );
			of.flush();
			of.close();
		}
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

		if ( CurrentQTable != -1 )
		{
			double delta = reward + gamma*bestFutureValue - ValueFunction[CurrentQTable]->GetValue( *PreviousState, *PreviousAction );

			for(int i = dataTrail.size()-1; i>=0; i--)
			{
				value = ValueFunction[CurrentQTable]->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
					+ ALPHA*delta*dataTrail[i].eligibilityTrace;

				ValueFunction[CurrentQTable]->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);

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
		else ai->utility->ChatMsg("RL:CRAP3");
	}


	if ( terminal )
	{
		goalAchieved = true;
		return NULL;//MEANS THAT YOU SHOULD STOP NOW!!
	}
	else
	{
		delete PreviousState;
		PreviousState = NULL;

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