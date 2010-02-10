#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"

using namespace springai;
using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:

		RL_State( AIClasses* ai, int node, unsigned short int type );
		RL_State();
		virtual ~RL_State();
		int GetID();
		vector<RL_Action> GetActions();
		void DeleteAction(int actionID);
		bool IsTerminal();
		int GetNode();
		bool operator==(const RL_State &other) const;
		RL_State & operator=(const RL_State &rhs);
	protected:
		bool terminal;
		AIClasses* ai;
		int ID;
		int Node;
		vector<RL_Action> Actions;

	};
}

#endif
