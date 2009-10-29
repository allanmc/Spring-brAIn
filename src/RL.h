#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_PLANT_INDEX 5
#define RL_ACTION_INDEX 4

#define GAMMA 0.9
#define ALPHA 1
#define FILE_HEADER "QB"
#include "global.h"
#include "RL_State.h"

using namespace springai;

namespace brainSpace {
	///This class has the responsibillty to choose the apropriate actions, when an event occurs.

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
	
	struct RL_Q
	{
		float* actionValueFunction;
		int size;
		char *File;
		int numStates;
		int numActions;
		AIClasses *ai;


		void LoadFromFile()
		{
			ifstream readFile;
			FileHeader fileHeader;
			FileHeaderQTable qTable;
			
			ai->utility->Log(ALL, LOG_RL, "I am going to read RL file!");

			readFile.open( File, ios::binary | ios::in );
			readFile.read( (char*)&fileHeader, sizeof(FileHeader) );
			if (fileHeader.header[0]==FILE_HEADER[0] &&
				fileHeader.header[1]==FILE_HEADER[1] &&
				fileHeader.type==1)
			{
				readFile.read( (char*)&qTable, sizeof(FileHeaderQTable) );//Only 1 in this flat q
				ai->utility->Log(ALL, LOG_RL, "FileHeaderQTable, numStates: %i", qTable.numStates);
				ai->utility->Log(ALL, LOG_RL, "FileHeaderQTable, numActions: %i", qTable.numActions);
				FileHeaderQAction qAction[qTable.numActions];
				for(int i = 0; i < qTable.numActions; i++)
				{
					ai->utility->Log(ALL, LOG_RL, "Loading qAction[%i]", i);
					qAction[i].LoadFromFile(ai, &readFile);
				}
				ai->utility->Log(ALL, LOG_RL, "Done reading qActions");
				readFile.read( (char*)actionValueFunction, sizeof(float)*qTable.numActions*qTable.numStates );
			}

			readFile.close();
		}

		RL_Q( AIClasses *aiClasses, int numStates, int numActions,const char *dir )
		{
			ai = aiClasses;
			this->numActions = numActions;
			this->numStates = numStates;
			size = numStates*numActions;
			actionValueFunction = new float[size];
			
			char filename[200];
			char *path = new char[200];
			strcpy(path, dir);
			SNPRINTF( filename, 200, "q.bin");
			strcat(path, filename);
			File = path;

			FILE* fp = NULL;
			fp = fopen( File, "rb" );
			if( fp != NULL )
			{
				fclose( fp );
				LoadFromFile();
			}
			else
			{
				for ( int i = 0 ; i < size ; i++ )
					actionValueFunction[i] = 0;
			}
		}

		~RL_Q()
		{
			delete[] actionValueFunction;
			delete[] File;
		}

		void SaveToFile()
		{
			FileHeader fileHeader;
			FileHeaderQTable qTable;
			FileHeaderQAction qAction[numActions];

			fileHeader.header[0] = FILE_HEADER[0];
			fileHeader.header[1] = FILE_HEADER[1];
			fileHeader.numQTables = 1;
			fileHeader.type = 1;
			qTable.numActions = numActions;
			qTable.numStates = numStates;
			qAction[0].id = 0;
			qAction[0].name = "armsolar";
			qAction[1].id = 1;
			qAction[1].name = "armmex";
			qAction[2].id = 2;
			qAction[2].name = "armlab";
			qAction[3].id = 3;
			qAction[3].name = "armvp";

			ofstream file( File, ios::binary | ios::out );
			file.write( (char*)&fileHeader, sizeof(fileHeader) );
			file.write( (char*)&qTable, sizeof(FileHeaderQTable) );
			//file.write( (char*)&qAction, sizeof(FileHeaderQAction)*numActions );
			for(int i = 0; i < numActions; i++)
				qAction[i].SaveToFile(&file);
			file.write( (char*)actionValueFunction, sizeof(float)*size );
			file.flush();
			file.close();
		}

		float GetValue( RL_State* state, RL_Action* action )
		{
			return actionValueFunction[ state->GetID()*(RL_ACTION_INDEX)+ action->ID ];
		}

		void SetValue( RL_State* state, RL_Action* action, float value )
		{
			actionValueFunction[state->GetID()*(RL_ACTION_INDEX)+ action->ID ] = value;
		}

	};

	class RL
	{
	public:
		RL( AIClasses* aiClasses);
		virtual ~RL();


		RL_Action *Update();
	private:

		bool FileExists( const char* name );
		RL_Action *FindNextAction( RL_State* state );
		RL_Action *FindBestAction( RL_State* state );

		RL_State* PreviousState;
		RL_Action* PreviousAction;
		int PreviousFrame;
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State* GetState();
		AIClasses* ai;

		RL_Q* ValueFunction;
	};
}

#endif