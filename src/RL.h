#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_ATTACK_BASE -1
#define RL_ATTACK_WEAK -2
#define RL_GUARD_COM -3
#define RL_GUARD_WEAK -4
#define RL_SCOUT -5

#define RL_Solar 0
#define RL_Mex 1
#define RL_Lab 2
#define RL_Plant 3
#define RL_flea 4
#define RL_rocko 5
#define RL_hammer 6
#define RL_flash 7
#define RL_jeffy 8
#define RL_shellshocker 9
#define RL_attack_base 10
#define RL_attack_weak 11
#define RL_scout 12

#define QBFILE_VERSION 2

#define RL_NUM_NODES 1

#define GAMMA 0.9
#define ALPHA 0.1
#define EPSILON 9

#define FILE_HEADER "QB"

#include "global.h"
#include "RL_State.h"
#include "RL_Q.h"

using namespace springai;

namespace brainSpace {	

	class RL
	{
	public:
		RL( AIClasses* aiClasses);
		virtual ~RL();
		RL_Action Update();
		void AddReward(float r);
		bool ShouldIUpdate();
	private:
		AIClasses* ai;
		
		RL_State nullState;
		RL_Action nullAction;

		bool FileExists( const char* name );
		RL_Action FindNextAction( RL_State &state );
		RL_Action FindBestAction( RL_State &state );

		int currentNode;
		vector<RL_State> PreviousState;
		vector<RL_Action> PreviousAction;
		vector<int> PreviousFrame;
		vector<int> ParentNode;
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State GetState(int node);
		RL_Action SafeNextAction(RL_State &state);
		void TakeAction(RL_Action &action);
		void ClearAllNodes();
		void LoadFromFile();
		void SaveToFile();
		float accumulatedReward;

		vector<RL_Q*> ValueFunction;
	};
}

#endif