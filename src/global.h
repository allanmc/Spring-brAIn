#ifndef _BRAIN_GLOBAL_H
#define _BRAIN_GLOBAL_H

namespace brainSpace {
	class Knowledge;
	class Utility;
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

#include "Utility.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

namespace brainSpace {
	typedef struct AIClasses {
		AICallback		*callback;
		Knowledge	*knowledge;
		Utility	*utility;
	} AIClasses;
}


#endif
