#include "RL_Q.h"
#include "RL.h"

using namespace brainSpace;

void RL_Q::LoadFromFile(ifstream *readFile)
{
	FileHeaderQTable qTable;
	readFile->read( (char*)&qTable, sizeof(FileHeaderQTable) );//Only 1 in this flat q
	ai->utility->Log(ALL, LOG_RL, "FileHeaderQTable, numStates: %i", qTable.numStates);
	ai->utility->Log(ALL, LOG_RL, "FileHeaderQTable, numActions: %i", qTable.numActions);
	FileHeaderQAction qAction[qTable.numActions];
	for(int i = 0; i < qTable.numActions; i++)
	{
		ai->utility->Log(ALL, LOG_RL, "Loading qAction[%i]", i);
		qAction[i].LoadFromFile(ai, readFile);
	}
	FileHeaderQStateVar qStateVars[qTable.numStateVars];
	for(int i = 0; i < qTable.numStateVars; i++)
	{
		ai->utility->Log(ALL, LOG_RL, "Loading qStateVar[%i]", i);
		qStateVars[i].LoadFromFile(ai, readFile);
	}
	ai->utility->Log(ALL, LOG_RL, "Done reading qActions");
	readFile->read( (char*)actionValueFunction, sizeof(float)*qTable.numActions*qTable.numStates );
}

RL_Q::RL_Q( AIClasses *aiClasses, vector<QAction> actions, vector<QStateVar> stateVars )
{
	ai = aiClasses;

	this->numActions = actions.size();
	int states = 1;
	for (int i = 0; i < stateVars.size(); i++)
	{
		states *= stateVars[i].numStates;
	}
	this->numStates =  states;
	this->actions = actions;
	this->stateVars = stateVars;
	
	size = this->numStates*this->numActions;
	actionValueFunction = new float[size];
}

RL_Q::~RL_Q()
{
	delete[] actionValueFunction;
	actionValueFunction = NULL;
	ai->utility->Log(ALL, LOG_RL, "Done deleting RL_Q");
}

void RL_Q::Clear()
{
	for ( int i = 0 ; i < size ; i++ )
		actionValueFunction[i] = 0;
}

void RL_Q::SaveToFile(ofstream *file )
{
	FileHeaderQTable qTable;
	FileHeaderQAction qAction[numActions];
	FileHeaderQStateVar fileQStateVar[stateVars.size()];
	qTable.numActions = numActions;
	qTable.numStates = numStates;
	qTable.numStateVars = stateVars.size();

	file->write( (char*)&qTable, sizeof(FileHeaderQTable) );
	for(int i = 0; i < actions.size(); i++)
	{
		qAction[i].qAction = actions[i];
		//qAction[i].qAction.id = i;
		//qAction[i].qAction.name = "action";
		qAction[i].SaveToFile(file);
	}
	for(int i = 0; i < stateVars.size(); i++)
	{
		fileQStateVar[i].qStateVar = stateVars[i];
		fileQStateVar[i].SaveToFile(file);
	}
	file->write( (char*)actionValueFunction, sizeof(float)*size );

}

float RL_Q::GetValue( RL_State &state, RL_Action &action )
{
	ai->utility->Log(ALL, MISC, "1 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action.ID, state.GetID());
	ai->utility->Log(ALL, MISC, "2 - RL_Q::GetValue(), acitonId = %i, stateID = %i, isterminal = %i", action.ID, state.GetID(), state.IsTerminal());
	ai->utility->Log(ALL, MISC, "3 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action.ID, state.GetID());
	int stateID = state.GetID();
	ai->utility->Log(ALL, MISC, "4 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action.ID, state.GetID());
	int actionID = action.ID;
	ai->utility->Log(ALL, MISC, "5 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action.ID, state.GetID());
	int index = stateID * numActions + actionID;
	ai->utility->Log(ALL, MISC, "6 - RL_Q::GetValue(), acitonId = %i, stateID = %i, index = %i", action.ID, state.GetID(), index);
	float retVal = actionValueFunction[ index ];
	ai->utility->Log(ALL, MISC, "7 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action.ID, state.GetID());
	return retVal;
}

void RL_Q::SetValue( RL_State &state, RL_Action &action, float value )
{
	actionValueFunction[ (state.GetID()) * numActions + (action.ID) ] = value;
}