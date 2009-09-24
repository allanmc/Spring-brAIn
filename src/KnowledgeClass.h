#ifndef _BRAINSPACE_KNOWLEDGE_H
#define _BRAINSPACE_KNOWLEDGE_H

#include "global.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class KnowledgeClass
	{
	public:
		KnowledgeClass( AIClasses* aiClasses );
		virtual ~KnowledgeClass();

	protected:

	private:
		AIClasses* AI;
	};
}
#endif
