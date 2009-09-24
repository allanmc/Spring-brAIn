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

	protected:
		AIClasses *ai;
		ResourceInfo *resourceInfo;
	};
}

#endif
