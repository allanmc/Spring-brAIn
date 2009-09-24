#ifndef _BRAINSPACE_GROUPCONTROLLER_H
#define _BRAINSPACE_GROUPCONTROLLER_H



#include "global.h"
#include "ConstructionGroupManager.h"
#include "MilitaryGroupManager.h"
#include "BrainGroup.h"

namespace brainSpace {
	class GroupController
	{
	public:
		GroupController( AIClasses* aiClasses );
		virtual ~GroupController(void);

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int ErectBuilding( SBuildUnitCommand order );
		
		void UnitIdle( Unit* unit );

		void ScoutWithIdleGroup();
		void AttackWithGroup(int enemy);

	private:
		ConstructionGroupManager* ConstructionGroupMgr;
		MilitaryGroupManager* MilitaryGroupMgr;
		AIClasses* AI;

	};
}

#endif // _BRAINSPACE_GROUPCONTROLLER_H
