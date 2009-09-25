#ifndef _BRAINSPACE_ARMYINFO_H
#define _BRAINSPACE_ARMYINFO_H

#include "global.h"
#include "QuadTree.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class ArmyInfo
	{
	public:
		ArmyInfo( AIClasses* aiClasses );
		virtual ~ArmyInfo();

		void AddUnit(Unit* unit);
		void RemoveUnit(Unit* unit);
		void UpdateUnit(Unit* unit);

		unsigned int unitCount;

	private:
		AIClasses* ai;
		map<int, SAIFloat3> positions;
		QuadTree* quadTree;
		
	};
}

#endif
