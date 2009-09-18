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
		GroupController( AICallback* callback );
		virtual ~GroupController(void);

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int ErectBuilding( SBuildUnitCommand order );

	private:
		ConstructionGroupManager* ConstructionGroupMgr;
		MilitaryGroupManager* MilitaryGroupMgr;
		AICallback* Callback;

	};
}

#endif // _BRAINSPACE_GROUPCONTROLLER_H
