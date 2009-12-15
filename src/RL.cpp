#include "RL.h"
#include <iostream>



using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses, unsigned short int type, double epsilon)
{	
	ai = aiClasses;
	EPSILON = epsilon;
	currentNode = 0;
	totalReward = 0;
	this->type = type;
	nullState = RL_State(ai, -1, type );
	nullAction = RL_Action( -1, -1, false );
	vector<QStateVar> stateVars;
	vector<QAction> actions;
	ai->utility->Log(ALL,MISC, "begin statevars");
	
	stateVars.push_back( QStateVar("CommanderDead", 3));
	stateVars.push_back( QStateVar("Rocko", RL_ROCKO_INDEX));
	stateVars.push_back( QStateVar("Lab", RL_LAB_INDEX));
	stateVars.push_back( QStateVar("Solar", RL_SOLAR_INDEX));
	stateVars.push_back( QStateVar("Mex", RL_MEX_INDEX));		
	actions.push_back( QAction("Lab", 0));
	actions.push_back( QAction("Solar", 1));
	actions.push_back( QAction("Mex", 2));
	actions.push_back( QAction("Rocko", 3));
	actions.push_back( QAction("Attack", 4));
	ValueFunction.push_back(new RL_Q(ai, actions, stateVars)); //root

	ClearAllNodes();
	dataTrail.clear();
	for (int i = 0 ; i < (int)ValueFunction.size() ; i++) 
	{
		PreviousFrame.push_back(0);
		if (i==0)
			ParentNode.push_back(-1); //no parent
		else
			ParentNode.push_back(0);
	}
	ai->utility->Log(ALL,MISC, "begin loadfromfile");
	LoadFromFile();

	totalReward = 0.0;
	buildingToBuild = 0;
	goalAchieved = false;
}

RL::~RL()
{

	ai->utility->Log(ALL, MISC, "Saving file");
	SaveToFile();
	ai->utility->Log(ALL, MISC, "File saved");
	for ( int i = 0 ; i < ValueFunction.size() ; i++ )
	{
		ai->utility->Log(ALL, MISC, "Deleting ValueFunction[%i]", i);
		delete ValueFunction[i];
		ValueFunction[i] = NULL;
		//delete PreviousAction[i];
		//PreviousAction[i] = NULL;
		//delete PreviousState[i];
		//PreviousState[i] = NULL;
	}
	ValueFunction.clear();
	dataTrail.clear();
	if (goalAchieved)
	{
		ai->utility->ChatMsg("RL goal achieved with total reward: %f", totalReward);
	} 
	else
	{
		ai->utility->ChatMsg("RL goal NOT achieved, but total reward is: %f", totalReward);
	}
	ai->utility->Log(ALL, MISC, "Done deconstructing RL");
}

void RL::ClearAllNodes()
{
	PreviousState.clear();
	PreviousAction.clear();
	for(unsigned int i = 0; i < ValueFunction.size(); i++)
	{
		PreviousState.push_back(nullState);
		PreviousAction.push_back( nullAction);
		ValueFunction[i]->Clear();
	}	
}

void RL::LoadFromFile()
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();
	delete dirs;

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path,"qn");
	char buffer[2];
	sprintf(buffer, "%d", ai->callback->GetTeamId());
	strcat(path, buffer);
	strcat(path, ".bin");

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
			fileHeader.type==QBFILE_VERSION)
		{
			for(int i = 0 ; i < fileHeader.numQTables; i++)
			{
				ValueFunction[i]->LoadFromFile(readFile);
			}
		}
		else
		{
			ai->utility->Log(ALL, LOG_RL, "Wrong/outdated RL file - so creating new!");
		}
		delete readFile;
	}
	delete[] path;
}

void RL::SaveToFile()
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();
	
	char *path = new char[200];
	strcpy(path, dir);
	strcat(path,"qn");
	char buffer[2];
	sprintf(buffer, "%d", ai->callback->GetTeamId());
	strcat(path, buffer);
	strcat(path, ".bin");

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
	delete dirs;
	delete[] path;
	delete file;
}

RL_State RL::GetState(int node)
{
	return RL_State(ai, node, type);
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
	if (action.ID < 0 || action.ID > 4)
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

void RL::setDesireToBuild(int buildingId)
{
	this->buildingToBuild = buildingId;
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
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() node:%d", currentNode);
	
	bool terminal = false;
	RL_State state = GetState(currentNode);
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() fail? state found");
	RL_Action nextAction = SafeNextAction(state);
	
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() nextAction set");

	//Start state
	if ( PreviousState[currentNode] == nullState )
	{
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() PreviousState == NULL, currentNode = %i, nextAction->Action = %i, nextAction->Complex = %i", currentNode, nextAction.Action, nextAction.Complex);
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = ai->frame;
		if(nextAction.Complex)
		{
			TakeAction(nextAction);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() doing recursive call 1");
			return Update();
		}
		else
			return nextAction;
	}//else

	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() before reward");

	//Reward
	float reward = 0;
	if (USE_RS_TIME) reward = PreviousFrame[currentNode] - ai->frame;
	if (PreviousAction[currentNode].Action == RL_ATTACK_ACTION)
	{
		reward = -10;
	}

	if(USE_RS_LABS && PreviousAction[currentNode].Action == ai->utility->GetUnitDef("armlab")->GetUnitDefId())
		reward += 10;

	float bestFutureValue;
	if ( state.IsTerminal() )
	{
		if(currentNode == 0 && ai->commanderDead == 1)
		{
			reward += 100;
		}
		else if(currentNode == 0 && ai->commanderDead == 2)
		{
			//we died.. bad boy
			reward -= 100;
		}

		terminal = true;
		bestFutureValue = reward;//no future actions to take
	}
	else
	{
		RL_Action bestAction = FindBestAction( state );
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() before getvalue");
		bestFutureValue = ValueFunction[currentNode]->GetValue(state, bestAction);
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() after getvalue");
	}
	if(currentNode == 0)
	{
		totalReward += reward;
	}

	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() reward set, %f", reward);
	float value;
	if(!USE_N_STEP && !USE_Q_LAMBDA)
	{
		//if complex then update the childs value funtion
		if(PreviousAction[currentNode].Complex)
		{
			int subNode = PreviousAction[currentNode].Action;
			float subValue = ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) 
				+ ALPHA*(
				reward + pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)ai->frame - (double)PreviousFrame[currentNode])))*bestFutureValue 
				- ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) );
			ValueFunction[subNode]->SetValue(PreviousState[subNode],PreviousAction[subNode], subValue);
		}
		//update own value function
		value = ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) 
			+ ALPHA*(
			reward + pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*(0.01*((double)ai->frame - (double)PreviousFrame[currentNode])))*bestFutureValue 
			- ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) );
		ValueFunction[currentNode]->SetValue(PreviousState[currentNode],PreviousAction[currentNode], value);
	}

	if(USE_BACKTRACKING)
	{
		//backtrack hack
		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			RL_Action bestAction = FindBestAction( dataTrail[i].resultState );
			float bestFutureValue = ValueFunction[currentNode]->GetValue(dataTrail[i].resultState, bestAction);
			value = ValueFunction[currentNode]->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
				+ ALPHA*(
				dataTrail[i].reward + pow((double)GAMMA, (USE_QSMDP?0:1)+(USE_QSMDP?1:0)*((0.01*(double)dataTrail[i].duration)))*bestFutureValue 
				- ValueFunction[currentNode]->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction) );
			ValueFunction[currentNode]->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);
		}
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState[currentNode], PreviousAction[currentNode], state, reward, ai->frame - PreviousFrame[currentNode]));
		if(BACKTRACKING_STEPS > 0 &&  dataTrail.size() > BACKTRACKING_STEPS)
			dataTrail.erase(dataTrail.begin());
	}
	if(USE_Q_LAMBDA )
	{
		//add the current to the dataTrail
		dataTrail.push_back(DataPoint(PreviousState[currentNode], PreviousAction[currentNode], state, reward, ai->frame - PreviousFrame[currentNode]));

		for ( int i = 0 ; i < dataTrail.size() ; i++ )
			if( dataTrail[i].eligibilityTrace < Q_LAMBDA_THRESHOLD)
				dataTrail.erase(dataTrail.begin());

		//RL_Action bestAction = FindBestAction( state );
		float delta = reward + GAMMA*bestFutureValue - ValueFunction[currentNode]->GetValue( PreviousState[currentNode], PreviousAction[currentNode] );

		for(int i = dataTrail.size()-1; i>=0; i--)
		{
			value = ValueFunction[currentNode]->GetValue(dataTrail[i].prevState, dataTrail[i].prevAction)
				+ ALPHA*delta*dataTrail[i].eligibilityTrace;
			ValueFunction[currentNode]->SetValue(dataTrail[i].prevState, dataTrail[i].prevAction, value);
			if ( m_greedyChoice )
			{
				dataTrail[i].eligibilityTrace *= GAMMA*LAMBDA;
			}
			else dataTrail[i].eligibilityTrace = 0;
		}

	}
	
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
		PreviousFrame[currentNode] = ai->frame;

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

bool brainSpace::RL::ShouldIUpdate()
{
	if(PreviousAction.empty())
	{
		ai->utility->ChatMsg("Wtf, previousaction is NULL?");
		return false;
	}
	
	if (PreviousAction[currentNode].Action == RL_ATTACK_ACTION)
	{
		//ai->utility->ChatMsg("We were attacking, idle?: %d", ai->knowledge->groupManager->GetMilitaryGroupMgr()->IsAllAttackGroupsIdle());
		//ai->utility->ChatMsg("We were attacking, attacking amount: %d", ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetNumAttackingGroups());

		return ai->knowledge->groupManager->GetMilitaryGroupMgr()->IsAllAttackGroupsIdle();

	} 
	else if (PreviousAction[currentNode].Action == ai->utility->GetUnitDef("armrock")->GetUnitDefId())
	{
		//ai->utility->ChatMsg("We were building rockos");
		map<int, struct UnitInformationContainer> units = ai->knowledge->selfInfo->baseInfo->GetUnits();
		//ai->knowledge->groupManager->GetMilitaryGroupMgr()->
		map<int, struct UnitInformationContainer>::iterator it;
		for(it = units.begin();it != units.end(); it++)
		{
			if(strcmp((*it).second.def->GetName(), "armlab") == 0)
			{
				if(Unit::GetInstance(ai->callback, (*it).first)->GetCurrentCommands().size() > 0)
					return false;
			}
		}
		return true;
	} 
	else
	{
		//ai->utility->ChatMsg("We were building a building, and command is idle: %d, commands: %d", ai->knowledge->groupManager->ConstructionGroupIsIdle(), ai->commander->GetCurrentCommands().size());
		if(ai->knowledge->groupManager->ConstructionGroupIsIdle())
			return true;
		else
			if (ai->commander->GetCurrentCommands().size() > 0)
			{
				return false;
			}
			else
			{
				return (ai->knowledge->groupManager->GetAmountOfBuildOrders() > 0);
			}
	}
	return false;
}