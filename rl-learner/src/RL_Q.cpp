#include "RL_Q.h"
#include "RL.h"

using namespace brainSpace;

double* RL_Q::actionValueFunction = NULL;

RL_Q::RL_Q(vector<QAction> actions, vector<QStateVar> stateVars )
{
	this->numActions = actions.size();
	int states = 1;
	for (int i = 0; i < (int)stateVars.size(); i++)
	{
		states *= stateVars[i].numStates;
	}
	this->numStates =  states;
	this->actions = actions;
	this->stateVars = stateVars;

	size = this->numStates*this->numActions;
	if(actionValueFunction == NULL)
	{
		actionValueFunction = new double[size];
		for ( int i = 0 ; i < size ; i++ )
			actionValueFunction[i] = 0;
	}
}

RL_Q::~RL_Q()
{

	//delete[] actionValueFunction;
	//actionValueFunction = NULL;
}

void RL_Q::Clear()
{
	for ( int i = 0 ; i < size ; i++ )
		actionValueFunction[i] = 0;
}

void RL_Q::LoadFromFile(ifstream *readFile, bool doIt)
{
	if (doIt)
	{
		FileHeaderQTable qTable;
		readFile->read( (char*)&qTable, sizeof(FileHeaderQTable) );//Only 1 in this flat q
		FileHeaderQAction *qAction = (FileHeaderQAction*)malloc(sizeof(FileHeaderQAction)* qTable.numActions);
		for(int i = 0; i < qTable.numActions; i++)
		{
			qAction[i].LoadFromFile( readFile);
		}
		FileHeaderQStateVar *qStateVars = (FileHeaderQStateVar*)malloc(sizeof(FileHeaderQStateVar)*qTable.numStateVars);
		for(int i = 0; i < qTable.numStateVars; i++)
		{
			qStateVars[i].LoadFromFile( readFile);
		}
		readFile->read( (char*)actionValueFunction, sizeof(double)*qTable.numActions*qTable.numStates );
		delete qAction;
		delete qStateVars;
	}
}

void RL_Q::SaveToFile(ofstream *file, bool doIt )
{
	if(doIt)
	{
		FileHeaderQTable qTable;
		FileHeaderQAction *qAction = (FileHeaderQAction*)malloc(sizeof(FileHeaderQAction)* numActions);
		FileHeaderQStateVar *fileQStateVar = (FileHeaderQStateVar*)malloc(sizeof(FileHeaderQStateVar)*stateVars.size());
		qTable.numActions = numActions;
		qTable.numStates = numStates;
		qTable.numStateVars = stateVars.size();

		file->write( (char*)&qTable, sizeof(FileHeaderQTable) );
		for(int i = 0; i < (int)actions.size(); i++)
		{
			qAction[i].qAction = actions[i];
			qAction[i].SaveToFile(file);
		}
		for(int i = 0; i < (int)stateVars.size(); i++)
		{
			fileQStateVar[i].qStateVar = stateVars[i];
			fileQStateVar[i].SaveToFile(file);
		}
		file->write( (char*)actionValueFunction, sizeof(double)*size );
		delete qAction;
		delete fileQStateVar;
	}
}

double RL_Q::GetValue( RL_State &state, RL_Action &action )
{
	int stateID = state.GetID();
	int actionID = action.ID;
	int index = stateID * numActions + actionID;
	double retVal = actionValueFunction[ index ];
	return retVal;
}

void RL_Q::SetValue( RL_State &state, RL_Action &action, double value )
{
	actionValueFunction[ (state.GetID()) * numActions + (action.ID) ] = value;
}