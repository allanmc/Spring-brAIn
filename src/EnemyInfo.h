#ifndef _BRAINSPACE_ENEMYINFO_H
#define _BRAINSPACE_ENEMYINFO_H

#include "global.h"

namespace brainSpace
{
	class EnemyInfo
	{
	public:
		EnemyInfo( AIClasses* aiClasses );
		virtual ~EnemyInfo();
		ArmyInfo* armyInfo;
		BaseInfo* baseInfo;

	protected:
		AIClasses *ai;
	};
}

#endif
