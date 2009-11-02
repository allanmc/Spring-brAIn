#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_PLANT_INDEX 5
#define RL_ACTION_INDEX 4

#define RL_NUM_NODES 3

#define GAMMA 0.9
#define ALPHA 1

#define FILE_HEADER "QB"

#include "global.h"
#include "RL_State.h"
#include "RL_Q.h"
#include "RL_State_Root.h"
#include "RL_State_Factory.h"
#include "RL_State_Resource.h"

using namespace springai;

namespace brainSpace {

	class RL
	{
	public:
		RL( AIClasses* aiClasses);
		virtual ~RL();


		RL_Action *Update();
	private:
		AIClasses* ai;

		bool FileExists( const char* name );
		RL_Action *FindNextAction( RL_State* state );
		RL_Action *FindBestAction( RL_State* state );

		int currentNode;
		RL_State* PreviousState[RL_NUM_NODES];
		RL_Action* PreviousAction[RL_NUM_NODES];
		int PreviousFrame[RL_NUM_NODES];
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State* GetState(int node);
		RL_Action* SafeNextAction(RL_Action *action);
		
		void LoadFromFile();
		void SaveToFile();

		RL_Q* ValueFunction[RL_NUM_NODES];
	};
}

#endif