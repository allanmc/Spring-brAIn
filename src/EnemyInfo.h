#ifndef _BRAINSPACE_ENEMYINFO_H
#define _BRAINSPACE_ENEMYINFO_H

#include "global.h"

using namespace std;
using namespace springai;

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
