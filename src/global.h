#ifndef _BRAIN_GLOBAL_H
#define _BRAIN_GLOBAL_H

#define LOG_TYPE CHAT|QUADTREE|DECISION|KNOWLEDGE|UTILITY
#define LOG_LEVEL ALL

//namespace brainSpace {
enum LogTypes {
	CHAT		= 1<<0,
	QUADTREE	= 1<<1,
	DECISION	= 1<<2,
	KNOWLEDGE	= 1<<3,
	UTILITY		= 1<<4,
	EVENT		= 1<<5
};



enum LogLevels {
	NONE		= 0,
	CRITICAL	= 1,
	IMPORTANT	= 2,
	DEBUG		= 3,
	ALL			= 4
};
//}

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
#include <math.h>

#include "Utility.h"
#include "Knowledge.h"
#include "ResourceInfo.h"
#include "SelfInfo.h"
#include "ArmyInfo.h"
#include "BaseInfo.h"
#include "EnemyInfo.h"

#endif
