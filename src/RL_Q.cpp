#include "RL_Q.h"
#include "RL.h"

using namespace brainSpace;
using namespace std;

void RL_Q::LoadFromFile(ifstream *readFile)
{
	FileHeaderQTable qTable;
	readFile->read( (char*)&qTable, sizeof(FileHeaderQTable) );//Only 1 in this flat q
	FileHeaderQAction *qAction = (FileHeaderQAction*)malloc(sizeof(FileHeaderQAction)* qTable.numActions);
	for(int i = 0; i < qTable.numActions; i++)
	{
		qAction[i].LoadFromFile( readFile, ai);
	}
	FileHeaderQStateVar *qStateVars = (FileHeaderQStateVar*)malloc(sizeof(FileHeaderQStateVar)*qTable.numStateVars);
	for(int i = 0; i < qTable.numStateVars; i++)
	{
		qStateVars[i].LoadFromFile( readFile, ai);
	}
	readFile->read( (char*)actionValueFunction, sizeof(double)*qTable.numActions*qTable.numStates );
	delete qAction;
	delete qStateVars;
}

RL_Q::RL_Q(AIClasses* aiClasses, vector<QAction> actions, vector<QStateVar> stateVars )
{
	ai = aiClasses;
	numActions = actions.size();
	int states = 1;
	for (unsigned int i = 0; i < stateVars.size(); i++)
	{
		states *= stateVars[i].numStates;
	}
	numStates =  states;
	Actions = actions;
	StateVars = stateVars;

	size = numStates*numActions;
	actionValueFunction = new double[size];
	for ( unsigned int i = 0 ; i < size ; i++ )
		actionValueFunction[i] = 0;


}

RL_Q::~RL_Q()
{
	delete[] actionValueFunction;
	actionValueFunction = NULL;
}

void RL_Q::Clear()
{
	for ( unsigned int i = 0 ; i < size ; i++ )
		actionValueFunction[i] = 0;
}

void RL_Q::SaveToFile(ofstream *file )
{	
	FileHeaderQTable qTable;
	FileHeaderQAction *qAction = (FileHeaderQAction*)malloc(sizeof(FileHeaderQAction)* numActions);
	FileHeaderQStateVar *fileQStateVar = (FileHeaderQStateVar*)malloc(sizeof(FileHeaderQStateVar)*StateVars.size());
	qTable.numActions = numActions;
	qTable.numStates = numStates;
	qTable.numStateVars = StateVars.size();
	file->write( (char*)&qTable, sizeof(FileHeaderQTable) );
	for(int i = 0; i < (int)Actions.size(); i++)
	{	
		qAction[i].qAction = Actions[i];
		qAction[i].SaveToFile(file, ai);
	}
	for(int i = 0; i < (int)StateVars.size(); i++)
	{
		fileQStateVar[i].qStateVar = StateVars[i];
		fileQStateVar[i].SaveToFile(file, ai);
	}
	file->write( (char*)actionValueFunction, sizeof(double)*size );
	delete qAction;
	delete fileQStateVar;
}

double RL_Q::GetValue( int stateID, RL_Action &action )
{
	int index = stateID * numActions + action.Action;
	double retVal = actionValueFunction[ index ];
	return retVal;
}


double RL_Q::GetValue( RL_State &state, RL_Action &action )
{
	int stateID = state.GetID();
	int actionID = action.Action;
	int index = stateID * numActions + actionID;
	double retVal = actionValueFunction[ index ];
	return retVal;
}

void RL_Q::SetValue( RL_State &state, RL_Action &action, double value )
{
	actionValueFunction[ (state.GetID()) * numActions + (action.Action) ] = value;
}