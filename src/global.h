#ifndef _BRAIN_GLOBAL_H
#define _BRAIN_GLOBAL_H

//#include "IncludesHeaders.h"

#include "Unit.h"
#include "UnitDef.h"
#include "Engine.h"
#include "Map.h"
#include "Resource.h"
#include "AICallback.h"
#include "SSkirmishAICallback.h"
#include "ExternalAI/Interface/AISCommands.h"
#include "OptionValues.h"
#include "Info.h"

namespace brainSpace {
	class Knowledge;
	class Utility;
	//struct AIClasses;

	class SelfInfo;
	class EnemyInfo;
	class ResourceInfo;
	class ArmyInfo;
	class BaseInfo;
}

using namespace std;
using namespace springai;
using namespace brainSpace;

namespace brainSpace {
	typedef struct AIClasses {
		AICallback *callback;
		Knowledge *knowledge;
		Utility *utility;
	} AIClasses;
}

struct CBoundingBox;
class QuadTreeNode;
class QuadTree;

#include <stdarg.h>
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <queue>
#include <list>
#include <stdlib.h>
#include <map>

#include "Utility.h"
#include "Knowledge.h"
#include "ResourceInfo.h"
#include "SelfInfo.h"
#include "ArmyInfo.h"
#include "BaseInfo.h"
#include "EnemyInfo.h"

#endif
