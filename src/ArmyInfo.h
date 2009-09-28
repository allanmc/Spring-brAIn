#ifndef _BRAINSPACE_ARMYINFO_H
#define _BRAINSPACE_ARMYINFO_H

class QuadTree;

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
		SAIFloat3 GetUnitPos(int unitId);
		unsigned int unitCount;

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		
	};
}

#endif
