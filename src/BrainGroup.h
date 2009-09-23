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
		BrainGroup( AICallback* callback );
		virtual ~BrainGroup();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int GetSize();
		bool IsIdle();



	protected:
		vector<Unit*> Units;
		bool Idle;
		AICallback* Callback;
	};
}

using namespace brainSpace;

#endif
