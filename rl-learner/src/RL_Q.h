#ifndef RL_Q_H
#define RL_Q_H

#include "RL_State.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string.h>
#include "RL_Action.h"

using namespace std;
namespace brainSpace
{
	struct QStateVar
	{
		const char *name;
		short unsigned int numStates;

		QStateVar(const char *n, short unsigned int ns)
		{
			name = n;
			numStates = ns;
		}
	};
	struct QAction
	{
		const char *name;
		short unsigned int id;
		QAction(const char *n, short unsigned int i)
		{
			name = n;
			id = i;
		}
	};

	struct FileHeaderQAction
	{
		short unsigned int nameSize;
		QAction qAction;

		void LoadFromFile(ifstream *file) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qAction.name = new char[nameSize];
			file->read( (char*)(qAction.name), sizeof(char)*nameSize );
			file->read( (char*)&(qAction.id), sizeof(short unsigned int) );
			delete[] qAction.name;
		}

		void SaveToFile(ofstream *file)
		{
			nameSize = strlen(qAction.name)+1;
			file->write( (char*)&nameSize, sizeof(short unsigned int) );
			file->write( (char*)(qAction.name), sizeof(char)*nameSize );
			file->write( (char*)&(qAction.id), sizeof(short unsigned int) );
		}
	};

	struct FileHeaderQStateVar
	{
		short unsigned int nameSize;
		QStateVar qStateVar;

		void LoadFromFile(ifstream *file) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qStateVar.name = new char[nameSize];
			file->read( (char*)(qStateVar.name), sizeof(char)*nameSize );
			file->read( (char*)&(qStateVar.numStates), sizeof(short unsigned int) );
			delete[] qStateVar.name;
		}

		void SaveToFile(ofstream *file)
		{
			nameSize = strlen(qStateVar.name)+1;
			file->write( (char*)&nameSize, sizeof(short unsigned int) );
			file->write( (char*)(qStateVar.name), sizeof(char)*nameSize );
			file->write( (char*)&(qStateVar.numStates), sizeof(short unsigned int) );
		}
	};

	struct FileHeaderQTable 
	{
		short unsigned int numStates;
		short unsigned int numActions;
		short unsigned int numStateVars;
	};

	struct FileHeader
	{
		char header[2];
		short unsigned int type;
		short unsigned int numQTables;
	};
	
	class RL_Q
	{
	private:
		double* actionValueFunction;
		int size;
		int numStates;
		int numActions;
		std::vector<QAction> actions;
		std::vector<QStateVar> stateVars;

	public:
		RL_Q( vector<QAction> actions, vector<QStateVar> stateVars );
		~RL_Q();

		void SaveToFile(ofstream *readFile);
		void LoadFromFile(ifstream *readFile);
		void Clear();
		double GetValue( RL_State &state, RL_Action &action );
		void SetValue( RL_State &state, RL_Action &action, double value );

	};

}
#endif

