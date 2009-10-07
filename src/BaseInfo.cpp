#include "BaseInfo.h"

using namespace brainSpace;
using namespace springai;

BaseInfo::BaseInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	CBoundingBox bbox;
	bbox.topLeft = (SAIFloat3){0,0,0};
	bbox.bottomRight = (SAIFloat3){	ai->callback->GetMap()->GetWidth()*8,
									0,
									ai->callback->GetMap()->GetHeight()*8};
	quadTree = new QuadTree( ai, bbox );
	buildingCount = 0;
	resourceBuildings = 0;
	productionBuildings = 0;
}

BaseInfo::~BaseInfo()
{
}

void BaseInfo::AddBuilding(Unit* building)
{
	//check for builder units
	if(building->GetDef()->GetSpeed() > 0) return;

	//ai->utility->ChatMsg("Adding base unit...");
	quadTree->InsertUnit(building->GetUnitId(), building->GetPos());

	buildingCount++;
	if(building->GetDef()->IsBuilder())
	{
		productionBuildings++;
	}
	else
	{
		resourceBuildings++;
	}
}

void BaseInfo::RemoveBuilding(Unit* building)
{
	//check for builder units
	if(building->GetDef()->GetSpeed() > 0) return;

	if (buildingCount==0)
	{
		return;
	}
	quadTree->RemoveUnit( building->GetUnitId() );
	//remove unit from quadtree, using pos

	buildingCount--;
	if(building->GetDef()->IsBuilder())
	{
		productionBuildings--;
	}
	else
	{
		resourceBuildings--;
	}
}

int BaseInfo::CountResourceBuildings()
{
	return resourceBuildings;
}


int BaseInfo::CountProductionBuildings()
{
	return productionBuildings;
}
