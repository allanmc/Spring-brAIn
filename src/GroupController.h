#ifndef _BRAINSPACE_GROUPCONTROLLER_H
#define _BRAINSPACE_GROUPCONTROLLER_H

#include "types.h"
#include "ConstructionGroupManager.h"
#include "MilitaryGroupManager.h"
#include "BrainGroup.h"

namespace brainSpace {
	///A controller that controls both the contruction- and military units
	class GroupController
	{
	public:
		GroupController( AIClasses* aiClasses );
		virtual ~GroupController(void);

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int ErectBuilding( SBuildUnitCommand order );
		
		void UnitIdle( Unit* unit );
		bool ConstructionGroupIsIdle();

		void ScoutWithIdleGroup();
		void AttackWithGroup(int enemy);
		void MoveGroupToPosition(SAIFloat3 pos);
		void AttackPositionWithAllGroups(SAIFloat3 pos);
	
		ConstructionGroupManager* GetConstructionGroupMgr();
		MilitaryGroupManager* GetMilitaryGroupMgr();

	private:
		ConstructionGroupManager* ConstructionGroupMgr;
		MilitaryGroupManager* MilitaryGroupMgr;
		AIClasses* ai;

	};
}

#endif // _BRAINSPACE_GROUPCONTROLLER_H
