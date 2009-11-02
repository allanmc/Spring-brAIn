#ifndef RL_STATE_RESOURCE_H
#define RL_STATE_RESOURCE_H

#include "types.h"
#include "RL_State.h"

namespace brainSpace
{

	class RL_State_Resource: public RL_State
	{
	public:
		RL_State_Resource(AIClasses *ai, int mexCount, int solarCount );
		RL_State_Resource(AIClasses *ai );
		~RL_State_Resource();
		bool IsTerminal();
	private:
		int MexCount;
		int SolarCount;
	};

}
#endif