#ifndef _BRAINSPACE_MILITARY_GROUP_MANAGER_H
#define _BRAINSPACE_MILITARY_GROUP_MANAGER_H

#include <set>
#include "BrainGroup.h"
#include "types.h"
#include "MilitaryUnitGroup.h"
#include "Point.h"

#define SCOUT_REWARD_DISTANCE 0.002
#define SCOUT_REWARD_AGE 0.0005
#define SCOUT_REWARD_THREAT 0.002
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
		void GuardUnit(Unit* unit);
		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		void GiveScoutOrder(MilitaryUnitGroup* group);
		void GiveAttackOrder(MilitaryUnitGroup* group, int enemy );
		vector<MilitaryUnitGroup*> GetAllAttackGroups();
		vector<MilitaryUnitGroup*> GetIdleAttackGroups();
		vector<MilitaryUnitGroup*> GetIdleScoutGroups();
		void UnitIdle(Unit* unit );
		bool IsAttackGroupAvailable();
		bool IsScoutGroupAvailable();
		void GiveMoveOrder(MilitaryUnitGroup* group, SAIFloat3 pos);
		int GetNumAttackingGroups();
		int GetNumScoutingGroups();
		bool IsAllAttackGroupsIdle();
		bool IsAllScoutGroupsIdle();
		
	private:
		vector<MilitaryUnitGroup*> UnitGroups;
		vector<MilitaryUnitGroup*> ScoutGroups;
		AIClasses* ai;
	};
}
#endif
