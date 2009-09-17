#ifndef _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H
#define _BRAINSPACE_CONSTRUCTION_GROUP_MANAGER_H


#include "Unit.h"
#include "UnitDef.h"
#include <set>

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


		void AddGroup();

	private:
		set<set<Unit*>*> UnitGroups;
		
	};
}


#endif