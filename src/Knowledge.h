#ifndef _BRAINSPACE_KNOWLEDGE_H
#define _BRAINSPACE_KNOWLEDGE_H

#include "global.h"
#include "MapInfo.h"
#include "GroupController.h"

namespace brainSpace
{
	///A collection of classes containing information needed to make decisions.
	class Knowledge
	{
	public:
		Knowledge( AIClasses* aiClasses );
		virtual ~Knowledge();
		SelfInfo* selfInfo;
		EnemyInfo* enemyInfo;
		MapInfo* mapInfo;
		GroupController* groupManager;

	protected:

	private:
		AIClasses* ai;
		
	};
}
#endif
