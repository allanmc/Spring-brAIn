#ifndef _BRAINSPACE_MILITARY_GROUP_MANAGER_H
#define _BRAINSPACE_MILITARY_GROUP_MANAGER_H

#include <set>
#include "BrainGroup.h"
#include "global.h"
#include "MilitaryUnitGroup.h"
#include "Point.h"

#define SCOUT_REWARD_DISTANCE 0.002
#define SCOUT_REWARD_AGE 0.0005
#define SCOUT_REWARD_THREAT 0.001
#define SCOUT_REWARD_START_POS 4
#define SCOUT_REWARD_METAL_SPOTS 1

using namespace springai;
using namespace std;
using namespace brainSpace;

namespace brainSpace
{

	///Controls all groups that are able to attack
	class MilitaryGroupManager
	{
	public:

		MilitaryGroupManager( AIClasses* aiClasses );
		virtual ~MilitaryGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		void GiveScoutOrder(MilitaryUnitGroup* group);
		void GiveAttackOrder(MilitaryUnitGroup* group, int enemy );
		vector<MilitaryUnitGroup*> GetIdleGroups();
		void UnitIdle(Unit* unit );
		vector<MilitaryUnitGroup*> GetNonAttackingGroups();

		
	private:
		vector<MilitaryUnitGroup*> UnitGroups;
		AIClasses* ai;
	};
}
#endif
