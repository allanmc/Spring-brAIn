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
	ai->utility->Log(ALL, LOG_RL, "Done reading qActions");
	readFile->read( (char*)actionValueFunction, sizeof(float)*qTable.numActions*qTable.numStates );
}

RL_Q::RL_Q( AIClasses *aiClasses, int numStates, int numActions )
{
	ai = aiClasses;
	this->numActions = numActions;
	this->numStates = numStates;
	size = numStates*numActions;
	actionValueFunction = new float[size];
}

RL_Q::~RL_Q()
{
	delete[] actionValueFunction;
	actionValueFunction = NULL;
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
	
	qTable.numActions = numActions;
	qTable.numStates = numStates;
	for(int i = 0; i< numActions; i++)
	{
		qAction[i].id = i;
		qAction[i].name = "p00p";
	}

	file->write( (char*)&qTable, sizeof(FileHeaderQTable) );
	//file.write( (char*)&qAction, sizeof(FileHeaderQAction)*numActions );
	for(int i = 0; i < numActions; i++)
		qAction[i].SaveToFile(file);
	file->write( (char*)actionValueFunction, sizeof(float)*size );

}

float RL_Q::GetValue( RL_State* state, RL_Action* action )
{
	ai->utility->ChatMsg("1 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action->ID, state->GetID());
	ai->utility->ChatMsg("2 - RL_Q::GetValue(), acitonId = %i, stateID = %i, isterminal = %i", action->ID, state->GetID(), state->IsTerminal());
	ai->utility->ChatMsg("3 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action->ID, state->GetID());
	int stateID = state->GetID();
	ai->utility->ChatMsg("4 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action->ID, state->GetID());
	int actionID = action->ID;
	ai->utility->ChatMsg("5 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action->ID, state->GetID());
	int index = stateID * numActions + actionID;
	ai->utility->ChatMsg("6 - RL_Q::GetValue(), acitonId = %i, stateID = %i, index = %i", action->ID, state->GetID(), index);
	float retVal = actionValueFunction[ index ];
	ai->utility->ChatMsg("7 - RL_Q::GetValue(), acitonId = %i, stateID = %i", action->ID, state->GetID());
	return retVal;
}

void RL_Q::SetValue( RL_State* state, RL_Action* action, float value )
{
	actionValueFunction[state->GetID()*(numActions)+ action->ID ] = value;
}
