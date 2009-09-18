#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include "Global.h"

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


		void AssignBuildOrder( SBuildUnitCommand order );


	protected:
		vector<Unit*> Units;
		bool Idle;
	private:
		AICallback* Callback;
	};
}

using namespace brainSpace;

#endif
