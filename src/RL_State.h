#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"

using namespace springai;
using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:
		RL_State( AIClasses* ai, int labCount, int solarCount, int mexCount );

		int GetID();

		vector<RL_Action*> GetActions();
		int LabCount;
	private:

		AIClasses* ai;
		
		int SolarCount;
		int MexCount;
		int ID;
		vector<RL_Action*> Actions;

	};
}

#endif
