#ifndef _BRAINSPACE_KNOWLEDGE_H
#define _BRAINSPACE_KNOWLEDGE_H

#include "global.h"

using namespace std;
using namespace springai;

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
	};
}
#endif
