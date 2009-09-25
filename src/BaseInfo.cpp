#include "BaseInfo.h"


using namespace brainSpace;
using namespace springai;

BaseInfo::BaseInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
}

BaseInfo::~BaseInfo()
{
}

void BaseInfo::AddBuilding(Unit* building)
{
	positions[building->GetUnitId()] = building->GetPos();

	//Add unit to quadtree, using pos

	buildingCount++;
}

void BaseInfo::RemoveBuilding(Unit* building)
{
	if (buildingCount==0)
	{
		return;
	}
	SAIFloat3 pos = positions[building->GetUnitId()];
	positions.erase(building->GetUnitId());

	//remove unit from quadtree, using pos

	buildingCount--;
}

