#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H

#include "global.h"

namespace brainSpace
{
	class Utility
	{
	private:
		AIClasses* ai;
		~Utility();

	public:
		Utility( AIClasses* aiClasses );

		void ChatMsg(const char* msg, ...);
		void ChatMsg(string msg);
		UnitDef* GetUnitDef(const char* unitDefName);
		Resource* GetResource(const char* resourceName);
	};
}
#endif
