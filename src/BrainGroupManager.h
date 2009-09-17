#ifndef _BRAINSPACE_BRAING_GROUP_MANAGER_H
#define _BRAINSPACE_BRAING_GROUP_MANAGER_H

#include "Unit.h"
#include "BrainGroup.h"
#include <vector>

using namespace std;
using namespace springai;


namespace brainSpace
{
	class BrainGroupManager
	{
	public:
		BrainGroupManager();
		virtual ~BrainGroupManager();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
	protected:
		vector<BrainGroup*> Groups;
	};
}
#endif
