#ifndef _BRAINSPACE_MILITARY_GROUP_MANAGER_H
#define _BRAINSPACE_MILITARY_GROUP_MANAGER_H

#include "Unit.h"
#include "UnitDef.h"
#include <vector>

using namespace springai;
using namespace std;


namespace brainSpace
{
	class MilitaryGroupManager
	{
	public:
		MilitaryGroupManager();
		virtual ~MilitaryGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );

		void AddGroup();
	private:
		vector<vector<Unit*>*> UnitGroups;
	};
}
#endif