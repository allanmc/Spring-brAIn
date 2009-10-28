#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_ACTION_INDEX 3

#define GAMMA 0.9
#define ALPHA 1
#define FILE_HEADER "QB"
#include "global.h"
#include "RL_State.h"

using namespace springai;

namespace brainSpace {
	///This class has the responsibillty to choose the apropriate actions, when an event occurs.

	struct FileHeader
	{
		char header[2];
		short unsigned int type; //1==flat, 2==hierarchical
		short unsigned int size;
	};

	struct RL_Q
	{
		float* actionValueFunction;
		int size;
		char *File;
		
		void LoadFromFile()
		{
			ifstream readFile;
			FileHeader fileHeader;
			readFile.open( File, ios::binary | ios::in );
			readFile.read( (char*)&fileHeader, sizeof(FileHeader) );
			if (fileHeader.header[0]==FILE_HEADER[0] &&
				fileHeader.header[1]==FILE_HEADER[1] &&
				fileHeader.type==1)
			{
				readFile.read( (char*)actionValueFunction, sizeof(float)*fileHeader.size );
			}

			readFile.close();
		}

		RL_Q( int numStates, int numActions,const char *dir )
		{
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
			fileHeader.header[0] = FILE_HEADER[0];
			fileHeader.header[1] = FILE_HEADER[1];
			fileHeader.type = 1;
			fileHeader.size = size;

			ofstream file( File, ios::binary | ios::out );
			file.write( (char*)&fileHeader, sizeof(fileHeader) );
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

		int Epsilon;
		RL_State* GetState();
		AIClasses* ai;

		vector<RL_Action*> Actions;
		RL_Q* ValueFunction;
	};
}

#endif