#ifndef _BRAINSPACE_KNOWLEDGE_H
#define _BRAINSPACE_KNOWLEDGE_H

#include "global.h"

namespace brainSpace
{
	class Knowledge
	{
	public:
		Knowledge( AIClasses* aiClasses );
		virtual ~Knowledge();

	protected:

	private:
		AIClasses* ai;
		SelfInfo* selfInfo;
	};
}
#endif
