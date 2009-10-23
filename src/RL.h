#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_ACTION_INDEX 4

#define GAMMA 0.9
#define ALPHA 1
#include "global.h"
#include "RL_State.h"

using namespace springai;

namespace brainSpace {
	///This class has the responsibillty to choose the apropriate actions, when an event occurs.


	struct RL_Q
	{
		float* actionValueFunction;

		RL_Q( int numStates, int numActions )
		{
			actionValueFunction = new float[numStates*numActions];
			for ( int i = 0 ; i < numStates*numActions ; i++ )
				actionValueFunction[i] = 0;
		}

		~RL_Q()
		{
			delete[] actionValueFunction;
		}

		void LoadFromFile( const char* file )
		{
			/*
			ifstream readFile;

			readFile.open( "q.bin", ios::binary | ios::in );

			for ( int i = 0 ; i < RL_ACTION_INDEX ; i++ )
			{
				for ( int j = 0 ; j < RL_SOLAR_INDEX*RL_MEX_INDEX*RL_LAB_INDEX ; j++ )
				{
					float f;
					readFile.read( (char*)&f, sizeof(float) );
					RL_State s( ai, j/400, (j%400)/20, j%20 );
					ValueFunction->SetValue( &s, Actions[i], f );
				}
			}
			readFile.close();
			*/
		}

		void SaveToFile( const char* file )
		{
			/*
			ofstream file( "q.bin", ios::binary | ios::out );
			for ( int i = 0 ; i < RL_ACTION_INDEX ; i++ )
			{
				for ( int j = 0 ; j < RL_SOLAR_INDEX ; j++ )
				{
					for ( int k = 0 ; k < RL_MEX_INDEX ; k++ )
				{
					for ( int l = 0 ; l < RL_LAB_INDEX ; l++ )
				{
					RL_State s( ai, l, j, k );
					RL_Action a(
					file.write( (char*)&Q[j][i], sizeof(float) );

				}
			
			}
			file.close();
			*/
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

		int Epsilon;
		RL_State* GetState();
		AIClasses* ai;

		vector<RL_Action*> Actions;
		RL_Q* ValueFunction;
	};
}

#endif