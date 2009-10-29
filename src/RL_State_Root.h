#ifndef RL_STATE_ROOT_H
#define RL_STATE_ROOT_H

#include "types.h"
#include "RL_State.h"

namespace brainSpace
{

	class RL_State_Root: public RL_State
	{
	public:
		RL_State_Root( AIClasses *ai, bool enoughLabs, bool canBuildLab );
		RL_State_Root( AIClasses *ai );
		~RL_State_Root();
		bool IsTerminal();
	private:
		bool EnoughLabs;
		bool CanBuildLab;
	};

}
#endif