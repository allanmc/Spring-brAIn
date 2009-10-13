#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include "global.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class BrainGroup
	{
	public:
		BrainGroup( AIClasses* aiClasses );
		virtual ~BrainGroup();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int GetSize();
		bool IsIdle();
		SAIFloat3 GetPos();



	protected:
		//<Unit, boolean> to indicate if the unit is idle
		map<Unit*, bool> Units;
		bool Idle;
		AIClasses* ai;
	};
}

using namespace brainSpace;

#endif
