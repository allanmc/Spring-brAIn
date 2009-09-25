#include "BaseInfo.h"

using namespace brainSpace;
using namespace springai;

BaseInfo::BaseInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	CBoundingBox bbox;
	bbox.topLeft = (SAIFloat3){0,0,0};
	bbox.bottomRight = (SAIFloat3){	ai->callback->GetMap()->GetWidth()*8,
									ai->callback->GetMap()->GetHeight()*8,
									0};
	quadTree = new QuadTree( bbox );
}

BaseInfo::~BaseInfo()
{
}

void BaseInfo::AddBuilding(Unit* building)
{
	positions[building->GetUnitId()] = building->GetPos();

	quadTree->InsertUnit(building->GetUnitId(), building->GetPos());

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

