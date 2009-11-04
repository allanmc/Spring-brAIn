#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"

using namespace springai;
using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:
		RL_State( AIClasses* ai, int node );
		virtual ~RL_State();
		int GetID();

		vector<RL_Action*> GetActions();
		void DeleteAction(RL_Action* action);
		bool IsTerminal();
		int GetNode();
	protected:
		bool terminal;
		AIClasses* ai;
		int ID;
		int Node;
		vector<RL_Action*> Actions;

	};
}

#endif
