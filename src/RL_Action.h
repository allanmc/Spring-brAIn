#ifndef RL_A_H
#define RL_A_H

#include "types.h"
#include "Unit.h"
#include "MilitaryUnitGroup.h"
#include "Path.h"
namespace brainSpace
{
struct RL_Action
	{
		int Action;
		std::vector<int> unitIDs;
		MilitaryUnitGroup* Group;
		//Necessary since Group pointer is deleted when the group is killed, and we still need to know how many units we lost
		int GroupSize;
		AIClasses* ai;
		brainSpace::Path* Path;
		
		RL_Action( AIClasses* aiClasses, int action, MilitaryUnitGroup* group, std::vector<int> ids, brainSpace::Path* path)
		{
			ai = aiClasses;
			Action = action;
			unitIDs = ids;
			Group = group;
			GroupSize = group->GetSize();
			Path = path;
		}
		RL_Action() 
		{
			Action = -1;
			Group = NULL;
			ai = NULL;
			Path = NULL;
		}
		
		bool operator==(const RL_Action &other) const {
			return (Action == other.Action);
		}

		~RL_Action()
		{
		}
	};
}
#endif
