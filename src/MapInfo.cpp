#include "MapInfo.h"

using namespace brainSpace;

MapInfo::MapInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	threatMap = new ThreatMap(ai);
	ai->utility->Log(ALL, MISC, "ThreatMap loaded...");
	scoutMap = new ScoutMap(ai);
	ai->utility->Log(ALL, MISC, "ScoutMap loaded...");
	resourceMap = new BrainResourceMap(ai);
	ai->utility->Log(ALL, MISC, "BrainResourceMap loaded...");
	pathfindingMap = new PathfindingMap( ai );
	ai->utility->Log(ALL, MISC, "PathfindingMap loaded...");
}

MapInfo::~MapInfo()
{
	delete threatMap;
	threatMap = NULL;
	delete scoutMap;
	scoutMap = NULL;
	delete resourceMap;
	resourceMap = NULL;
	delete pathfindingMap;
	pathfindingMap = NULL;
}

