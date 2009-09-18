#ifndef _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H
#define _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H


#include "global.h"
#include "ConstructionUnitGroup.h"


using namespace brainSpace;
using namespace springai;
using namespace std;

namespace brainSpace
{
	class ConstructionGroupManager
	{
	public:
		ConstructionGroupManager( AICallback* callback );
		virtual ~ConstructionGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );

		int DelegateBuildOrder( SBuildUnitCommand order );
	private:
		vector<ConstructionUnitGroup*> UnitGroups;
		AICallback* Callback;
		
	};
}


#endif