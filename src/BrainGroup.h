#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include "global.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	///A group of units, used for both combat and construction units.
	class BrainGroup
	{
	public:
		BrainGroup( AIClasses* aiClasses, int groupID );
		virtual ~BrainGroup();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int GetSize();
		bool IsIdle();
		SAIFloat3 GetPos();
		int GetGroupID();



	protected:
		//<Unit, boolean> to indicate if the unit is idle
		map<Unit*, bool> Units;
		bool Idle;
		AIClasses* ai;
		int GroupID;
	};
}

using namespace brainSpace;

#endif
