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
	return actionValueFunction[ state->GetID()*(numActions)+ action->ID ];
}

void RL_Q::SetValue( RL_State* state, RL_Action* action, float value )
{
	actionValueFunction[state->GetID()*(numActions)+ action->ID ] = value;
}
