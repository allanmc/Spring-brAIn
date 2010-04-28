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

#define GROUP_PROXIMITY 500

namespace brainSpace
{

	///Controls all groups that are able to attack
	class MilitaryGroupManager
	{
	public:
		

		MilitaryGroupManager( AIClasses* aiClasses );
		virtual ~MilitaryGroupManager();
		void GuardUnit(Unit* unit);
		bool AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );

		void GiveScoutOrder(MilitaryUnitGroup* group);
		void GiveAttackOrder(MilitaryUnitGroup* group, int enemy );
		void GiveAttackOrder(MilitaryUnitGroup* group, vector<int> enemies );

		vector<MilitaryUnitGroup*> GetAllAttackGroups();
		vector<MilitaryUnitGroup*> GetIdleAttackGroups();
		vector<MilitaryUnitGroup*> GetIdleScoutGroups();
		bool UnitIdle(Unit* unit );
		bool IsAttackGroupAvailable();
		bool IsScoutGroupAvailable();
		void GiveMoveOrder(MilitaryUnitGroup* group, SAIFloat3 pos);
		int GetNumAttackingGroups();
		int GetNumScoutingGroups();
		bool IsAllAttackGroupsIdle();
		bool IsAllScoutGroupsIdle();
		MilitaryUnitGroup* GetNearestGroup( MilitaryUnitGroup* group );
		
	private:
		vector<MilitaryUnitGroup*> UnitGroups;
		vector<MilitaryUnitGroup*> ScoutGroups;
		AIClasses* ai;
	};
}
#endif
