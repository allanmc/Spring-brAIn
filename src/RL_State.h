#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H


#include "global.h"

using namespace springai;

namespace brainSpace {
	///This class has the responsibillty to choose the apropriate actions, when an event occurs.


	class RL_State
	{
	public:
		RL_State( AIClasses* ai, int labCount, int solarCount, int mexCount );

		int GetID();

		vector<RL_Action> GetActions();

	private:

		AIClasses* ai;
		int LabCount;
		int SolarCount;
		int MexCount;
		int ID;
		vector<RL_Action> Actions;


	};
}