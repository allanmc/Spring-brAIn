#ifndef _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H
#define _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H


#include "global.h"
#include "ConstructionUnitGroup.h"


using namespace brainSpace;
using namespace springai;
using namespace std;

namespace brainSpace
{
	///A manager of all contruction groups
	class ConstructionGroupManager
	{
	public:
		ConstructionGroupManager( AIClasses* aiClasses );
		virtual ~ConstructionGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );

		int DelegateBuildOrder( SBuildUnitCommand order );
		bool IsIdle();
		void UnitIdle( Unit* unit );
	private:
		vector<ConstructionUnitGroup*> UnitGroups;
		AIClasses* ai;
		
	};
}


#endif
