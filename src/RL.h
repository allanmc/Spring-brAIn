#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_PLANT_INDEX 5

#define GAMMA 0.9
#define ALPHA 1
#define FILE_HEADER "QB"
#include "global.h"
#include "RL_State.h"

using namespace springai;

namespace brainSpace {
	///This class has the responsibillty to choose the apropriate actions, when an event occurs.
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
	
	struct RL_Q
	{
		float* actionValueFunction;
		int size;
		char *File;
		int numStates;
		int numActions;
		AIClasses *ai;
		std::vector<QAction> actions;
		std::vector<QStateVar> stateVars;

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
				FileHeaderQStateVar qStateVars[qTable.numStateVars];
				for(int i = 0; i < qTable.numStateVars; i++)
				{
					ai->utility->Log(ALL, LOG_RL, "Loading qStateVar[%i]", i);
					qStateVars[i].LoadFromFile(ai, &readFile);
				}
				ai->utility->Log(ALL, LOG_RL, "Done reading qActions");
				readFile.read( (char*)actionValueFunction, sizeof(float)*qTable.numActions*qTable.numStates );
			}

			readFile.close();
		}

		RL_Q( AIClasses *aiClasses, vector<QAction> actions, vector<QStateVar> stateVars )
		{
			//DataDirs::GetInstance(ai->callback)->GetWriteableDir()
			ai = aiClasses;

			const char* dir = ai->callback->GetDataDirs()->GetWriteableDir();
			
			ai->utility->Log(ALL, MISC, "RL_Q 1");
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
			ai->utility->Log(ALL, MISC, "RL_Q 2");
			actionValueFunction = new float[size];
			ai->utility->Log(ALL, MISC, "RL_Q 3");
			char filename[200];
			char *path = new char[200];
			strcpy(path, dir);
			SNPRINTF( filename, 200, "q.bin");
			strcat(path, filename);
			File = path;
			ai->utility->Log(ALL, MISC, "RL_Q 4");
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
			ai->utility->Log(ALL, MISC, "RL_Q 5");
		}

		~RL_Q()
		{
			delete[] actionValueFunction;
			delete[] File;
		}

		void SaveToFile()
		{
			FileHeader fileHeader;
			fileHeader.header[0] = FILE_HEADER[0];
			fileHeader.header[1] = FILE_HEADER[1];
			fileHeader.numQTables = 1;
			fileHeader.type = 1;
			
			FileHeaderQTable qTable;
			FileHeaderQAction qAction[numActions];
			FileHeaderQStateVar fileQStateVar[stateVars.size()];
			qTable.numActions = numActions;
			qTable.numStates = numStates;
			qTable.numStateVars = stateVars.size();

			ofstream file( File, ios::binary | ios::out );
			file.write( (char*)&fileHeader, sizeof(fileHeader) );
			file.write( (char*)&qTable, sizeof(FileHeaderQTable) );

			
			for(int i = 0; i < actions.size(); i++)
			{
				qAction[i].qAction = actions[i];
				qAction[i].SaveToFile(&file);
			}
			for(int i = 0; i < stateVars.size(); i++)
			{
				fileQStateVar[i].qStateVar = stateVars[i];
				fileQStateVar[i].SaveToFile(&file);
			}
			file.write( (char*)actionValueFunction, sizeof(float)*size );
			file.flush();
			file.close();
		}

		float GetValue( RL_State* state, RL_Action* action )
		{
			return actionValueFunction[ state->GetID()*(numActions)+ action->ID ];
		}

		void SetValue( RL_State* state, RL_Action* action, float value )
		{
			actionValueFunction[state->GetID()*(numActions)+ action->ID ] = value;
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
		int totalReward;
		bool goalReached;
		int Epsilon;
		RL_State* GetState();
		AIClasses* ai;

		RL_Q* ValueFunction;
	};
}

#endif