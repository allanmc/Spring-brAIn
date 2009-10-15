#include "MapInfo.h"

using namespace brainSpace;

MapInfo::MapInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	threatMap = new ThreatMap(ai);
	scoutMap = new ScoutMap(ai);
	resourceMap = new BrainResourceMap(ai);
	pathfindingMap = new PathfindingMap( ai );
}

MapInfo::~MapInfo()
{
	delete(threatMap);
	delete(scoutMap);
	delete(resourceMap);
}

