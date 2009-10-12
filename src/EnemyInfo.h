#ifndef _BRAINSPACE_ENEMYINFO_H
#define _BRAINSPACE_ENEMYINFO_H

#include "global.h"

namespace brainSpace
{
	///A collection of classes with information on the enemy.
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
