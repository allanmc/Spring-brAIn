#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"

using namespace springai;
using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:
		RL_State( AIClasses* ai );
		virtual ~RL_State();
		int GetID();

		vector<RL_Action*> GetActions();
		virtual bool IsTerminal()=0;
	protected:

		AIClasses* ai;
		int ID;
		vector<RL_Action*> Actions;

	};
}

#endif
