#ifndef _BRAINSPACE_KNOWLEDGE_H
#define _BRAINSPACE_KNOWLEDGE_H

#include "global.h"
#include "MapInfo.h"

namespace brainSpace
{
	class Knowledge
	{
	public:
		Knowledge( AIClasses* aiClasses );
		virtual ~Knowledge();
		SelfInfo* selfInfo;
		EnemyInfo* enemyInfo;
		MapInfo* mapInfo;

	protected:

	private:
		AIClasses* ai;
		

	};
}
#endif
