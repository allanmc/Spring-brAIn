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

	struct RL_Action
	{
		int UnitDefID;
		int ID;
		RL_Action( int unitDefID, int id )
		{
			UnitDefID = unitDefID;
			ID = id;
		}
		
	};



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
		}

		float GetValue( RL_State* state, RL_Action* action )
		{
			return actionValueFunction[ state->GetID()*(RL_ACTION_INDEX)+ action->ID ];
		}

		float SetValue( RL_State* state, RL_Action* action, float value )
		{
			actionValueFunction[state->GetID()*(RL_ACTION_INDEX)+ action->ID ] = value;
		}

	};

	class RL
	{
	public:
		RL( AIClasses* aiClasses);
		virtual ~RL();


		int Update();
	private:

		bool FileExists( const char* name );
		int FindNextAction( int state );
		int FindBestAction( int state );

		RL_State* PreviousState;
		RL_Action* PreviousAction;

		int Epsilon;
		RL_State* GetState();
		AIClasses* ai;

		vector<RL_Action> Actions;
		RL_Q* ValueFunction;
	};
}

#endif