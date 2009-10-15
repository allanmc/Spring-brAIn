#ifndef _BRAIN_TYPES_H
#define _BRAIN_TYPES_H

#include <windows.h>
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

#include "Unit.h"
#include "UnitDef.h"
#include "DataDirs.h"
#include "Engine.h"
#include "Map.h"
#include "Resource.h"
#include "AICallback.h"
#include "SSkirmishAICallback.h"
#include "OptionValues.h"
#include "Info.h"
#include "ExternalAI/Interface/AISCommands.h"
#include "Point.h"
#include "Movedata.h"
//namespace brainSpace {
enum LogTypes {
	CHAT		= 1<<0,
	QUADTREE	= 1<<1,
	DECISION	= 1<<2,
	KNOWLEDGE	= 1<<3,
	UTILITY		= 1<<4,
	EVENT		= 1<<5,
	BN			= 1<<6,
	SCOUTING	= 1<<7,
	KMEDOIDS	= 1<<8,
	GROUPING	= 1<<9,
	MISC		= 1<<10,
	SLOPEMAP	= 1<<11
};



enum LogLevels {
	NONE		= 0,
	CRITICAL	= 1,
	IMPORTANT	= 2,
	DEBUG		= 3,
	ALL			= 4
};
//}

namespace brainSpace {
	class Knowledge;
	class Utility;
	//struct AIClasses;

	class SelfInfo;
	class EnemyInfo;
	class ResourceInfo;
	class ArmyInfo;
	class BaseInfo;
	class BrainMath;
	struct CBoundingBox;
	class BrainMap;
}

namespace brainSpace {
	///A container for misc classes that all classes should know. (used due to inabillity to use Singletons)
	struct AIClasses {
		springai::AICallback *callback;
		Knowledge *knowledge;
		Utility *utility;
		BrainMath* math;
		int frame;
	};

	struct CBoundingBox;
	class QuadTreeNode;
	class QuadTree;
}

#include "Utility.h"

#endif
