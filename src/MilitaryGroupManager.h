#ifndef _BRAINSPACE_MILITARY_GROUP_MANAGER_H
#define _BRAINSPACE_MILITARY_GROUP_MANAGER_H

#include "Unit.h"
#include "UnitDef.h"
#include <set>
#include "BrainGroup.h"
#include "global.h"
#include "MilitaryUnitGroup.h"

using namespace springai;
using namespace std;
using namespace brainSpace;

namespace brainSpace
{
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
		AIClasses* AI;
	};
}
#endif
