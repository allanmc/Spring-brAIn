#ifndef RL_Q_H
#define RL_Q_H



#include "types.h"
#include "RL_State.h"

namespace brainSpace
{

	struct FileHeaderQAction
	{
		short unsigned int id;
		short unsigned int nameSize;
		const char *name;

		void LoadFromFile(AIClasses *ai, ifstream *file) 
		{
			file->read( (char*)&id, sizeof(short unsigned int) );
			file->read( (char*)&nameSize, sizeof(short unsigned int) );
			name = new char[nameSize];
			file->read( (char*)name, sizeof(char)*nameSize );
			
		}

		void SaveToFile(ofstream *file)
		{
			nameSize = strlen(name)+1;
			file->write( (char*)&id, sizeof(short unsigned int) );
			file->write( (char*)&nameSize, sizeof(short unsigned int) );
			file->write( (char*)name, sizeof(char)*nameSize );
		}
	};

	struct FileHeaderQTable 
	{
		short unsigned int numStates;
		short unsigned int numActions;
	};

	struct FileHeader
	{
		char header[2];
		short unsigned int type; //1==flat, 2==hierarchical
		short unsigned int numQTables;
	};
	
	class RL_Q
	{
	private:
		float* actionValueFunction;
		int size;
		int numStates;
		int numActions;
		AIClasses *ai;

	public:
		RL_Q( AIClasses *aiClasses, int numStates, int numActions);
		~RL_Q();

		void SaveToFile(ofstream *readFile);
		void LoadFromFile(ifstream *readFile);
		void Clear();
		float GetValue( RL_State &state, RL_Action &action );
		void SetValue( RL_State &state, RL_Action &action, float value );

	};

}
#endif

