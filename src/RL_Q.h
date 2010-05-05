#ifndef RL_Q_H
#define RL_Q_H

#include "types.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>

namespace brainSpace
{
	class RL_State;
	struct RL_Action;
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

		void LoadFromFile(std::ifstream *file, AIClasses* aiClasses) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qAction.name = new char[nameSize];
			file->read( (char*)(qAction.name), sizeof(char)*nameSize );
			file->read( (char*)&(qAction.id), sizeof(short unsigned int) );
			delete[] qAction.name;
			//delete[] qAction.name;
		}

		void SaveToFile(std::ofstream *file, AIClasses* aiClasses)
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

		void LoadFromFile(std::ifstream *file, AIClasses* aiClasses) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qStateVar.name = new char[nameSize];
			file->read( (char*)(qStateVar.name), sizeof(char)*nameSize );
			file->read( (char*)&(qStateVar.numStates), sizeof(short unsigned int) );
			delete[] qStateVar.name;
		}

		void SaveToFile(std::ofstream *file, AIClasses* aiClasses)
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
		unsigned int size;
		unsigned int numStates;
		unsigned int numActions;
		std::vector<QAction> Actions;
		std::vector<QStateVar> StateVars;
		AIClasses* ai;

	public:
		RL_Q( AIClasses* aiClasses, std::vector<QAction> actions, std::vector<QStateVar> stateVars );
		~RL_Q();

		void SaveToFile(std::ofstream *readFile);
		void LoadFromFile(std::ifstream *readFile);
		void Clear();
		double GetValue( int stateID, RL_Action &action );
		
		double GetValue( RL_State &state, RL_Action &action );
		void SetValue( RL_State &state, RL_Action &action, double value );

	};

}
#endif

