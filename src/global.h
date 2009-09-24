#ifndef _BRAIN_GLOBAL_H
#define _BRAIN_GLOBAL_H

namespace brainSpace {
	class KnowledgeClass;
	class UtilityClass;
	struct AIClasses;
}

#include "Unit.h"
#include "UnitDef.h"
#include "Engine.h"
#include "Map.h"
#include "Resource.h"
#include "AICallback.h"
#include "SSkirmishAICallback.h"
#include "ExternalAI/Interface/AISCommands.h"
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <queue>
#include <stdlib.h>

#include "UtilityClass.h"
#include "KnowledgeClass.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

namespace brainSpace {
	typedef struct AIClasses {
		AICallback		*Callback;
		KnowledgeClass	*Knowledge;
		UtilityClass	*Utility;
	} AIClasses;
}


#endif
