#ifndef RL_Q_H
#define RL_Q_H



#include "types.h"
#include "RL_State.h"

namespace brainSpace
{

	struct QStateVar
	{
		const char *name;
		short unsigned int numStates;
	};
	struct QAction
	{
		const char *name;
		short unsigned int id;
	};

	struct FileHeaderQAction
	{
		short unsigned int nameSize;
		QAction qAction;

		void LoadFromFile(AIClasses *ai, ifstream *file) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qAction.name = new char[nameSize];
			file->read( (char*)(qAction.name), sizeof(char)*nameSize );
			file->read( (char*)&(qAction.id), sizeof(short unsigned int) );
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

		void LoadFromFile(AIClasses *ai, ifstream *file) 
		{
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			qStateVar.name = new char[nameSize];
			file->read( (char*)(qStateVar.name), sizeof(char)*nameSize );
			file->read( (char*)&(qStateVar.numStates), sizeof(short unsigned int) );
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
		float* actionValueFunction;
		int size;
		int numStates;
		int numActions;
		std::vector<QAction> actions;
		std::vector<QStateVar> stateVars;
		AIClasses *ai;

	public:
		RL_Q( AIClasses *aiClasses, vector<QAction> actions, vector<QStateVar> stateVars );
		~RL_Q();

		void SaveToFile(ofstream *readFile);
		void LoadFromFile(ifstream *readFile);
		void Clear();
		float GetValue( RL_State &state, RL_Action &action );
		void SetValue( RL_State &state, RL_Action &action, float value );

	};

}
#endif

