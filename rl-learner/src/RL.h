#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 20
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_PLANT_INDEX 5

#define QBFILE_VERSION 2

#define RL_NUM_NODES 1

#define GAMMA 0.9
#define ALPHA 0.1
#define EPSILON 0.1

#define FILE_HEADER "QB"

#include "RL_State.h"
#include "RL_Q.h"
#include "game.h"
#include <vector>
#include "RL_Action.h"

using namespace std;

namespace brainSpace {	

	class RL
	{
	public:
		RL(Game *g);
		virtual ~RL();
		float GetTotalReward();


		RL_Action Update();
	private:
		Game *game;
		RL_State nullState;
		RL_Action nullAction;

		bool FileExists( const char* name );
		RL_Action FindNextAction( RL_State &state );
		RL_Action FindBestAction( RL_State &state );

		int currentNode;
		RL_State PreviousState[RL_NUM_NODES];
		RL_Action PreviousAction[RL_NUM_NODES];
		int PreviousFrame[RL_NUM_NODES];
		int ParentNode[RL_NUM_NODES];
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State GetState(int node);
		RL_Action SafeNextAction(RL_State &state);
		void TakeAction(RL_Action &action);
		void ClearAllNodes();
		void LoadFromFile();
		void SaveToFile();

		RL_Q* ValueFunction[RL_NUM_NODES];
	};
}

#endif