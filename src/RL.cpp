#include "RL.h"
#include <iostream>



using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{	
	ai = aiClasses;
	accumulatedReward = 0;
	currentNode = 0;
	totalReward = 0;
	nullState = RL_State(ai, -1 );
	nullAction = RL_Action( -1, -1, false );
	vector<QStateVar> stateVars;
	vector<QAction> actions;
	ai->utility->Log(ALL,MISC, "begin statevars");

	stateVars.push_back( QStateVar("eap/odp", 4));
	stateVars.push_back( QStateVar("oap/edp", 4));
	stateVars.push_back( QStateVar("Mexprof", 4));
	stateVars.push_back( QStateVar("Enerprof", 4));
	stateVars.push_back( QStateVar("scoutknow", 3));
	stateVars.push_back( QStateVar("productionPower", 5));
	stateVars.push_back( QStateVar("CommanderDead", 3));
	ai->utility->Log(ALL,MISC, "begin actions");
	actions.push_back( QAction("Solar", 0));
	actions.push_back( QAction("Mex", 1));
	actions.push_back( QAction("Lab", 2));
	actions.push_back( QAction("Plant", 3));
	actions.push_back( QAction("flea", 4));
	actions.push_back( QAction("rocko", 5));
	actions.push_back( QAction("hammer", 6));
	actions.push_back( QAction("flash", 7));
	actions.push_back( QAction("jeffy", 8));
	actions.push_back( QAction("shellshocker", 9));
	actions.push_back( QAction("attack base", 10));
	actions.push_back( QAction("attack weak", 11));
	//actions.push_back( QAction("guard commander", 12));
	//actions.push_back( QAction("guard weak", 13));
	actions.push_back( QAction("scout", 12));
	ai->utility->Log(ALL,MISC, "begin valuefunc");
	ValueFunction.push_back(new RL_Q( ai, actions, stateVars)); //root	

	ai->utility->Log(ALL,MISC, "begin frame");
	for (int i = 0 ; i < (int)ValueFunction.size() ; i++) 
	{
		PreviousFrame.push_back(0);
		if (i==0)
			ParentNode.push_back(-1); //no parent
		else
			ParentNode.push_back(0);
	}
	ai->utility->Log(ALL,MISC, "begin clear");	
	ClearAllNodes();
	ai->utility->Log(ALL,MISC, "begin loadfromfile");
	LoadFromFile();

	totalReward = 0.0;
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
	return RL_State(ai, node);
}

RL_Action RL::FindNextAction( RL_State &state )
{
	vector<RL_Action> stateActions = state.GetActions();
	RL_Action action = stateActions[0]; //unitdefID
	
	int r = rand()%100;
	if ( r <= EPSILON ) //non-greedy
	{
		action = stateActions[rand()%stateActions.size()];
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: actionID=%d unitdef=%d", action.ID, action.Action );
	}
	else //greedy
	{
		action = FindBestAction(state);
		ai->utility->Log( ALL, LOG_RL, "Greedy: actionID=%d unitdef=%d", action.ID, action.Action );
	}
	return action;
}

RL_Action RL::FindBestAction( RL_State &state )
{
	ai->utility->Log(ALL, MISC, "Calling GetActions");
	vector<RL_Action> stateActions = state.GetActions();
	ai->utility->Log(ALL, MISC, "Done calling GetActions.size = %i", stateActions.size());

	RL_Action action = stateActions[0]; //unitdefID
	ai->utility->Log(ALL, MISC, "-Hax action: action->ID = %i, action->Action = %i", stateActions[0].ID, stateActions[0].Action);
	ai->utility->Log(ALL, MISC, "-Current action: action->ID = %i, action->Action = %i", action.ID, action.Action);
	

	float bestValue = ValueFunction[currentNode]->GetValue(state, action);

	ai->utility->Log(ALL, MISC, "Hax action: action->ID = %i, action->Action = %i", stateActions[0].ID, stateActions[0].Action);

	//vector<RL_Action*>::iterator it;
	//for ( it = stateActions.begin()+1 ; it != stateActions.end() ; it++ )
	for ( int i = 1 ; i < (int)stateActions.size() ; i++ )
	{
		//RL_Action *tempAction = (RL_Action*)(*it);
		RL_Action tempAction = stateActions[i];
		ai->utility->Log(ALL, MISC, "Current action: action->ID = %i, action->Action = %i", tempAction.ID, tempAction.Action);
		ai->utility->Log(ALL, MISC, "Hax action: action->ID = %i, action->Action = %i", stateActions[0].ID, stateActions[0].Action);
		float tempValue = ValueFunction[currentNode]->GetValue(state, tempAction);
		ai->utility->Log(ALL, MISC, "Current action: action->ID = %i, action->Action = %i", tempAction.ID, tempAction.Action);
		ai->utility->Log(ALL, MISC, "Hax action: action->ID = %i, action->Action = %i", stateActions[0].ID, stateActions[0].Action);
		
		if ( tempValue > bestValue )
		{
			bestValue = tempValue;
			action = tempAction;
		}
	}
	ai->utility->Log(ALL, MISC, "Hax action: action->ID = %i, action->Action = %i", stateActions[0].ID, stateActions[0].Action);
	ai->utility->Log(ALL, MISC, "Reutning from GetAction, with action->ID = %i, action->Action = %i", action.ID, action.Action);
	return action;
}

RL_Action RL::SafeNextAction(RL_State &state)
{
	RL_State tmpCurrentState = state;
	//int tmpCurrentNode = currentNode;

	while(state.GetActions().size() > 0)
	{
		RL_Action nextAction = FindNextAction( state );
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:SafeNextAction action found checking for complexity: %d", nextAction.Complex);
		if(nextAction.Complex)
		{
			tmpCurrentState = GetState(nextAction.Action);
			if(tmpCurrentState.GetActions().size() == 0)
			{
				state.DeleteAction(nextAction.ID);
				ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:SafeNextAction deleted action");
			}
			else
			{
				ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:SafeNextAction complex action found");
				return nextAction;
			}
		}
		else
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:SafeNextAction non-complex action found");
			return nextAction;
		}
	}
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:SafeNextAction ... bad stuff happend!");

	return nullAction;
}

void RL::TakeAction(RL_Action &action)
{
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:TakeAction() action->Compelx:%i", action.Complex);
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:TakeAction() action->Action:%i", action.Action);
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:TakeAction() action->ID:%i", action.ID);
	//should only be called with complex actions!
	if(!action.Complex)
		return;
	
	currentNode = action.Action;
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:TakeAction() new node:%d", currentNode);
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
	if(PreviousAction[currentNode].ID < 10)
	{
		reward -= (ai->frame - PreviousFrame[currentNode]) / 30.0;
	}
	reward += accumulatedReward;
	accumulatedReward = 0;
	float bestFutureValue;
	if ( state.IsTerminal() )
	{
		ai->utility->Log(LOG_DEBUG, LOG_RL, "terminal set");
		if(ai->commanderDead == -1)
		{
			//not good, bad reward
			reward -= 1000;
		}
		else if(ai->commanderDead == 1)
		{
			//great, we won!
			reward += 1000;
		}
		terminal = true;
		bestFutureValue = 0;//no future actions to take
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

 	//if complex then update the childs value funtion
	if(PreviousAction[currentNode].Complex)
	{
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() complex Previousaction 1");
		int subNode = PreviousAction[currentNode].Action;
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() complex Previousaction 2, currentNode = %i, previousaction->action = %i, previousaction-id = %i", currentNode, subNode, PreviousAction[currentNode].ID);
		float subValue = ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) 
						+ ALPHA*(
							reward + GAMMA*bestFutureValue 
							- ValueFunction[subNode]->GetValue(PreviousState[subNode],PreviousAction[subNode]) );
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() complex Previousaction 3");
		ValueFunction[subNode]->SetValue(PreviousState[subNode],PreviousAction[subNode], subValue);
	}
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() complex Previousaction done");
	//update own value function
	float value = ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) 
				+ ALPHA*(
					reward + GAMMA*bestFutureValue 
					- ValueFunction[currentNode]->GetValue(PreviousState[currentNode],PreviousAction[currentNode]) );
	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() calcualted value");
	ValueFunction[currentNode]->SetValue(PreviousState[currentNode],PreviousAction[currentNode], value);


	ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() value function updated");
	
	if ( terminal )
	{
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() terminal");
		if(ParentNode[currentNode] == -1)//root check
		{
			goalAchieved = true;
			return nullAction;//MEANS THAT YOU SHOULD STOP NOW!!
		}
		else
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "changing to parent: %d",ParentNode[currentNode]);
			currentNode = ParentNode[currentNode];//parentNode
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() doing recursive call 2");
			return Update();//recursive call
		}
	}
	else
	{
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() non terminal");
		PreviousState[currentNode] = state;
		PreviousAction[currentNode] = nextAction;
		PreviousFrame[currentNode] = ai->frame;
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() done setting stuff");
		
		if(nextAction.Complex)
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() complex");
			TakeAction(nextAction);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() doing recursive call 3");
			return Update();
		}
		else
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() non complex");
			return nextAction;
		}
	}
}

void RL::AddReward(float r)
{
	accumulatedReward += r;
}

bool RL::ShouldIUpdate()
{
	switch(PreviousAction[currentNode].ID)
	{
	case RL_Solar:
		{
		}
	case RL_Mex:
		{
		}
	case RL_Lab:
		{
		}
	case RL_Plant:
		{
			return ai->knowledge->groupManager->ConstructionGroupIsIdle();
		}
	case RL_flea:
	case RL_rocko:
	case RL_hammer:
		{
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
	case RL_flash:
	case RL_jeffy:
	case RL_shellshocker:
		{
			map<int, struct UnitInformationContainer> units = ai->knowledge->selfInfo->baseInfo->GetUnits();
			map<int, struct UnitInformationContainer>::iterator it;
			for(it = units.begin();it != units.end(); it++)
			{
				if(strcmp((*it).second.def->GetName(), "armvp") == 0)
				{
					if(Unit::GetInstance(ai->callback, (*it).first)->GetCurrentCommands().size() > 0)
						return false;
				}
			}
			return true;
		}
	case RL_attack_base:
	case RL_attack_weak:
		{
			if(ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetNumAttackingGroups() > 0)
				return false;
			return true;
		}
	case RL_scout:
		{
			if(ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetNumScoutingGroups() > 0)
				return false;
			return true;
		}
	default:
		{
			return true;
		}
	}
}