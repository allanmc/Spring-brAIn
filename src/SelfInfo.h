#ifndef _BRAINSPACE_SELFINFO_H
#define _BRAINSPACE_SELFINFO_H

#include "global.h"

namespace brainSpace
{
	class SelfInfo
	{
	public:
		SelfInfo(AIClasses *ai);
		virtual ~SelfInfo();
		ResourceInfo *resourceInfo;
		ArmyInfo* armyInfo;
		BaseInfo* baseInfo;

	protected:
		AIClasses *ai;

	};
}

#endif
