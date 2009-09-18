#ifndef _BRAINSPACE_MILITARY_GROUP_MANAGER_H
#define _BRAINSPACE_MILITARY_GROUP_MANAGER_H

#include "Unit.h"
#include "UnitDef.h"
#include <set>
#include "BrainGroup.h"

using namespace springai;
using namespace std;
using namespace brainSpace;

namespace brainSpace
{
	class MilitaryGroupManager
	{
	public:
		MilitaryGroupManager();
		virtual ~MilitaryGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );

	private:
		vector<BrainGroup*> UnitGroups;
	};
}
#endif