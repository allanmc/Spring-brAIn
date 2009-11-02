#ifndef RL_STATE_FACTORY_H
#define RL_STATE_FACTORY_H

#include "types.h"
#include "RL_State.h"

namespace brainSpace
{

	class RL_State_Factory: public RL_State
	{
	public:
		RL_State_Factory(AIClasses *ai, int labCount, int plantCount );
		RL_State_Factory(AIClasses *ai );
		~RL_State_Factory();
		bool IsTerminal();
	private:
		int LabCount;
		int PlantCount;
	};

}
#endif