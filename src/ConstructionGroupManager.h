#ifndef _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H
#define _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H


#include "Unit.h"
#include "UnitDef.h"
#include <set>
#include "Utility.h"


using namespace brainSpace;
using namespace springai;
using namespace std;

namespace brainSpace
{
	class ConstructionGroupManager
	{
	public:
		ConstructionGroupManager();
		virtual ~ConstructionGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		
	};
}


#endif