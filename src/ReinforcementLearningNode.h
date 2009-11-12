#ifndef _BRAINSPACE_RLNODE_H
#define _BRAINSPACE_RLNODE_H

#include "global.h"
#include "RL_State.h"
#include "RL_Q.h"


using namespace springai;
using namespace std;

namespace brainSpace {	

	RL_State nullState;
	RL_Action nullAction;

	class ReinforcementLearningNode
	{
	public:
		ReinforcementLearningNode( AIClasses* aiClasses, vector<QAction> actions, vector<QStateVar> states, ReinforcementLearningNode* parent );
		virtual ~ReinforcementLearningNode();

		RL_Action Update();


	private:

		AIClasses* ai;

		RL_Action CalcNextAction( RL_State &state );
		RL_Action CalcBestAction( RL_State &state );
		RL_Action CalcSafeNextAction( RL_State &state );

		virtual RL_State GetState()=0;
		virtual float GetReward()=0;
		void UpdateValueFunction();
		ReinforcementLearningNode* Parent;
		RL_Q* ValueFunction;


		RL_State PreviousState;
		RL_Action PreviousAction;
		int PreviousFrame;
	};
}
#endif